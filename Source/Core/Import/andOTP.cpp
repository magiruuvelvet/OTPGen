#include "andOTP.hpp"

#include <iostream>

#include <Core/TokenDatabase.hpp>

#include <cereal/external/rapidjson/document.h>
#include <cereal/external/rapidjson/memorystream.h>
#include <cereal/external/rapidjson/stringbuffer.h>
#include <cereal/external/rapidjson/writer.h>

#include <cryptopp/sha.h>
#include <cryptopp/filters.h>

#include <gcrypt.h>

// andOTP token entry schema
// JSON array
//
// see also: https://github.com/andOTP/andOTP/wiki/Special-features
//
// {
//     "secret": "",
//     "label": "",
//     "period": 30,
//     "digits": 6,
//     "type": "TOTP/HOTP/STEAM",
//     "algorithm": "SHA1",
//     "thumbnail": "Default",
//     "last_used": 0,
//     "tags": []
// }

namespace Import {

const uint8_t andOTP::ANDOTP_IV_SIZE = 12U;
const uint8_t andOTP::ANDOTP_TAG_SIZE = 16U;

void andOTP::gcrypt_init()
{
    static bool initialized = false;
    if (!initialized)
    {
        gcry_check_version(GCRYPT_VERSION);
        gcry_control(GCRYCTL_DISABLE_SECMEM, 0);
        gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);
        initialized = true;
    }
}

bool andOTP::importTokens(const std::string &file, std::vector<OTPToken*> &target, const Type &type, const std::string &password)
{
    // read file contents into memory
    std::string out;
    auto status = TokenDatabase::readFile(file, out);
    if (status != TokenDatabase::Success)
    {
        out.clear();
        return false;
    }

    if (out.empty())
    {
        return false;
    }

    // decrypt contents first if they are encrypted
    if (type == Encrypted)
    {
        std::string decrypted;
        auto status = decrypt(password, out, decrypted);
        if (status)
        {
            out = decrypted;
            decrypted.clear();
        }
        else
        {
            out.clear();
            decrypted.clear();
            return false;
        }
    }

    // parse json
    try {
        rapidjson::StringStream s(out.c_str());
        rapidjson::Document json;
        json.ParseStream(s);

        // root element must be an array
        if (!json.IsArray())
        {
            return false;
        }

        auto array = json.GetArray();
        for (auto&& elem : array)
        {
            // check if object has all andOTP members
            if (!(elem.HasMember("type") &&
                elem.HasMember("secret") &&
                elem.HasMember("label")/* &&
                elem.HasMember("period") &&
                elem.HasMember("digits") &&
                elem.HasMember("algorithm")*/))
            {
                continue;
            }

            try {

            const auto typeStr = std::string(elem["type"].GetString());

            if (typeStr == "TOTP")
            {
                auto token = new TOTPToken();
                token->_secret = elem["secret"].GetString();
                token->_label = elem["label"].GetString();
                token->_period = elem["period"].GetUint();
                token->_digits = static_cast<OTPToken::DigitType>(elem["digits"].GetUint());
                token->setAlgorithm(elem["algorithm"].GetString());
                target.push_back(token);
            }
            else if (typeStr == "HOTP")
            {
                auto token = new HOTPToken();
                token->_secret = elem["secret"].GetString();
                token->_label = elem["label"].GetString();
                token->_counter = elem["counter"].GetUint();
                token->_digits = static_cast<OTPToken::DigitType>(elem["digits"].GetUint());
                token->setAlgorithm(elem["algorithm"].GetString());
                target.push_back(token);
            }
            else if (typeStr == "STEAM")
            {
                auto token = new SteamToken();
                token->_secret = elem["secret"].GetString();
                token->_label = elem["label"].GetString();
                target.push_back(token);
            }
            else
            {
                continue;
            }

            } catch (...) {
                continue;
            }
        }
    } catch (...) {
        // catch all rapidjson exceptions
        return false;
    }

    return true;
}

bool andOTP::exportTokens(const std::string &target, const std::vector<OTPToken*> &tokens)
{
    try {
        rapidjson::Document json(rapidjson::kArrayType);

        for (auto&& token : tokens)
        {
            rapidjson::Value value(rapidjson::kObjectType);
            value.AddMember("secret", rapidjson::Value(token->secret().c_str(), json.GetAllocator()), json.GetAllocator());
            value.AddMember("label", rapidjson::Value(token->label().c_str(), json.GetAllocator()), json.GetAllocator());
            value.AddMember("period", token->period(), json.GetAllocator());
            value.AddMember("digits", token->digits(), json.GetAllocator());

            if (token->type() == OTPToken::HOTP)
            {
                value.AddMember("type", "HOTP", json.GetAllocator());
            }
            else if (token->type() == OTPToken::Steam)
            {
                value.AddMember("type", "STEAM", json.GetAllocator());
            }
            else
            {
                value.AddMember("type", "TOTP", json.GetAllocator());
            }

            if (token->type() == OTPToken::Steam)
            {
                value.AddMember("algorithm", "SHA1", json.GetAllocator());
                value["digits"].SetUint(5);
            }
            else
            {
                value.AddMember("algorithm", rapidjson::Value(token->algorithmString().c_str(), json.GetAllocator()), json.GetAllocator());
            }

            value.AddMember("thumbnail", "Default", json.GetAllocator());
            value.AddMember("last_used", 0, json.GetAllocator());
            value.AddMember("tags", rapidjson::Value(rapidjson::kArrayType), json.GetAllocator());
            json.PushBack(value, json.GetAllocator());
        }

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        json.Accept(writer);

        const auto res = TokenDatabase::writeFile(target, buffer.GetString());
        if (res != TokenDatabase::Success)
        {
            return false;
        }

    } catch (...) {
        return false;
    }

    return true;
}

const std::string andOTP::sha256_password(const std::string &password)
{
    if (password.empty())
        return password;

    std::string hashed_password;

    // don't use smart pointers here, already managed/deleted by crypto++ itself
    CryptoPP::SHA256 hash;
    CryptoPP::StringSource src(password, true,
        new CryptoPP::HashFilter(hash,
            new CryptoPP::StringSink(hashed_password)));

    return hashed_password;
}

bool andOTP::decrypt(const std::string &password, const std::string &buffer, std::string &decrypted)
{
    // stream too small
    if (buffer.size() <= (ANDOTP_IV_SIZE + ANDOTP_TAG_SIZE))
    {
        return false;
    }

    try {
        const auto stream_size = buffer.size();
        const auto iv = buffer.substr(0, ANDOTP_IV_SIZE);
        const auto tag = buffer.substr(stream_size - ANDOTP_TAG_SIZE);

        const auto enc_buf_size = stream_size - ANDOTP_IV_SIZE - ANDOTP_TAG_SIZE;
        const auto enc_buf = buffer.substr(ANDOTP_IV_SIZE, enc_buf_size);

        // does crypto++ support GCM?

        gcrypt_init();

        gcry_cipher_hd_t hd;
        gcry_cipher_open(&hd, GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_GCM, GCRY_CIPHER_SECURE);
        gcry_cipher_setkey(hd, sha256_password(password).c_str(), gcry_cipher_get_algo_keylen(GCRY_CIPHER_AES256));
        gcry_cipher_setiv(hd, iv.c_str(), ANDOTP_IV_SIZE);
        auto decrypted_json = gcry_calloc_secure(enc_buf_size, 1);
        gcry_cipher_decrypt(hd, decrypted_json, enc_buf_size, enc_buf.c_str(), enc_buf_size);
        if (gcry_err_code(gcry_cipher_checktag(hd, tag.c_str(), ANDOTP_TAG_SIZE)) == GPG_ERR_CHECKSUM)
        {
            gcry_cipher_close(hd);
            return false;
        }

        gcry_cipher_close(hd);

        decrypted = std::string(static_cast<char*>(decrypted_json),
                                static_cast<char*>(decrypted_json) + std::strlen(static_cast<char*>(decrypted_json)));
        std::free(decrypted_json);

        // clean up stream, remove possible garbage data from end
        auto json_end = decrypted.find_last_of(']');
        if (json_end == std::string::npos)
        {
            decrypted.clear();
            return false;
        }

        decrypted.resize(json_end + 1);
    } catch (...) {
        return false;
    }

    return true;
}

}
