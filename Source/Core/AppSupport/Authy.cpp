#include "Authy.hpp"

#include <iostream>

#include <TokenDatabase.hpp>

#include <cereal/external/rapidjson/document.h>
#include <cereal/external/rapidjson/memorystream.h>

#include <cereal/external/rapidxml/rapidxml.hpp>

#include <cryptopp/algparam.h>
#include <cryptopp/hex.h>
#include <cryptopp/base32.h>
#include <cryptopp/filters.h>

// Authy TOTP tokens
// =================
//
// XML:
//  <map>
//    <string name="com.authy.storage.tokens.authenticator.key">
//      xml-escaped json string
//    </string
//  </map>
//
// JSON:
//  > array of:
//  {
//    "accountType": "",
//    "decryptedSecret": "",    <-- base-32 secret,
//    "digits": 0,
//    "encryptedSecret": "",    <-- pretty useless when the secret is stored decrypted in the same file
//    "originalName": "",
//    "timestamp": 0,
//    "salt": "",               <-- encryptedSecret salt (useless as decrypted secret is stored in the same file)
//    "upload_state": "",
//    "hidden": false,
//    "id": "",
//    "isNew": false,
//    "name": ""                <-- label
//  }
//

// Authy native tokens
// ===================
//
// XML:
//  <map>
//    <string name="com.authy.storage.tokens.authy.key">
//      xml-escaped json string
//    </string>
//  </map>
//
// JSON:
//  > array of:
//  {
//    "appid": {...},       // not interesting for importing
//    "assetData": {...},   // ---
//    "assetGroup": "",
//    "digits": 0,
//    "lastCheck": "",
//    "secretSeed": "",     <-- hex encoded string, must be re-encoded into base-32
//    "serialId": 0,
//    "version": 0,
//    "hidden": false,
//    "id": "",
//    "isNew": false,
//    "name": ""            <-- label
//  }
//

namespace AppSupport {

bool Authy::importTOTP(const std::string &file, std::vector<TOTPToken> &target, const Format &format)
{
    std::string json;
    auto status = prepare(file, format, TOTP, json);
    if (!status)
    {
        return status;
    }

    // parse json
    try {
        rapidjson::StringStream s(json.c_str());
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
            // check if object has all required members
            if (!(elem.HasMember("decryptedSecret") &&
                elem.HasMember("digits") &&
                elem.HasMember("name")))
            {
                continue;
            }

            TOTPToken token;
            token.setSecret(elem["decryptedSecret"].GetString());
            token.setLabel(elem["name"].GetString());
            token.setDigitLength(static_cast<OTPToken::DigitType>(elem["digits"].GetUint()));
            target.push_back(token);
        }
    } catch (...) {
        // catch all rapidjson exceptions
        return false;
    }

    return true;
}

bool Authy::importNative(const std::string &file, std::vector<AuthyToken> &target, const Format &format)
{
    std::string json;
    auto status = prepare(file, format, Native, json);
    if (!status)
    {
        return status;
    }

    // parse json
    try {
        rapidjson::StringStream s(json.c_str());
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
            // check if object has all required members
            if (!(elem.HasMember("secretSeed") &&
                elem.HasMember("digits") &&
                elem.HasMember("name")))
            {
                continue;
            }

            AuthyToken token;
            token.setSecret(hexToBase32Rfc4648(elem["secretSeed"].GetString()));
            token.setLabel(elem["name"].GetString());
            token.setDigitLength(static_cast<OTPToken::DigitType>(elem["digits"].GetUint()));
            target.push_back(token);
        }
    } catch (...) {
        // catch all rapidjson exceptions
        return false;
    }

    return true;
}

bool Authy::prepare(const std::string &file, const Format &format, const AuthyXMLType &type, std::string &json)
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

    if (format == XML)
    {
        auto status = extractJSON(out, type, json);
        if (!status)
        {
            out.clear();
            return status;
        }
    }
    else
    {
        json = out;
    }

    out.clear();
    return true;
}

const std::string Authy::hexToBase32Rfc4648(const std::string &hex)
{
    // create an RFC 4648 base-32 encoder
    // crypto++ uses DUDE by default which isn't TOTP compatible
    auto encoder = new CryptoPP::Base32Encoder();
    static const CryptoPP::byte ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    static const CryptoPP::AlgorithmParameters params = CryptoPP::MakeParameters(
                                                        CryptoPP::Name::EncodingLookupArray(),
                                                        static_cast<const CryptoPP::byte*>(ALPHABET));
    encoder->IsolatedInitialize(params);

    // raw pointers are automatically deleted by crypto++
    std::string base32;
    encoder->Attach(new CryptoPP::StringSink(base32));
    CryptoPP::StringSource src(hex, true,
          new CryptoPP::HexDecoder(encoder));
    return base32;
}

bool Authy::extractJSON(const std::string &xml, const AuthyXMLType &type, std::string &json)
{
    const std::string attr = type == TOTP ?
        "com.authy.storage.tokens.authenticator.key" :
        "com.authy.storage.tokens.authy.key";

    try {
        cereal::rapidxml::xml_document<> doc;
        auto buf = xml;
        doc.parse<0>(const_cast<char*>(buf.data()));

        auto map = doc.first_node("map", 3, false);
        if (!map) return false;

        auto string = map->first_node("string", 6, false);
        if (!string) return false;

        auto name = string->first_attribute("name", 4, false);
        if (!name) return false;

        const auto authy_type = std::string(name->value());
        if (authy_type != attr)
        {
            return false;
        }

        const auto json_value = std::string(string->value());
        json = json_value;
    } catch (...) {
        return false;
    }

    return true;
}

}
