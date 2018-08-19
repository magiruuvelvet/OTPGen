#include "TokenDatabase.hpp"
#include "TokenStore.hpp"

#include <fstream>
#include <ostream>
#include <sstream>

#include <cryptopp/cryptlib.h>
#include <cryptopp/algparam.h>
#include <cryptopp/aes.h>
#include <cryptopp/sha.h>
#include <cryptopp/base64.h>
#include <cryptopp/hkdf.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>

#include <cereal/types/vector.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/portable_binary.hpp>

namespace {
    static const std::string TOKEN_ARCHIVE_MAGIC = "OTPTokenArchive";
    static const uint32_t TOKEN_ARCHIVE_VERSION = 0x01;
}

std::string TokenDatabase::password;
std::string TokenDatabase::tokenFile;

bool TokenDatabase::setPassword(const std::string &password)
{
    if (password.empty())
        return false;

    // remove old password
    TokenDatabase::password.clear();

    // don't use smart pointers here, already managed/deleted by crypto++ itself
    CryptoPP::SHA256 hash;
    CryptoPP::StringSource src(password, true,
        new CryptoPP::HashFilter(hash,
            new CryptoPP::Base64Encoder(
                new CryptoPP::StringSink(TokenDatabase::password))));

    return true;
}

bool TokenDatabase::setTokenFile(const std::string &file)
{
    if (file.empty())
        return false;

    tokenFile = file;
    return true;
}

TokenDatabase::Error TokenDatabase::changePassword(const std::string &newPassword)
{
    Error status = Success;

    if (newPassword.empty())
    {
        return PasswordEmpty;
    }

    auto pwdStatus = setPassword(newPassword);
    if (!pwdStatus)
    {
        return PasswordHashFailure;
    }

    status = saveTokens();
    return status;
}

struct TokenData
{
    OTPToken::TokenType type;
    OTPToken::Label label;
    OTPToken::SecretType secret;
    OTPToken::DigitType digits;
    OTPToken::PeriodType period;
    OTPToken::CounterType counter;
    OTPToken::ShaAlgorithm algorithm;

    template<class Archive>
    void serialize(Archive &ar)
    {
        ar(type, label, secret, digits, period, counter, algorithm);
    }
};

// Container to store the entire token store in a single file.
struct TokenDataAdapter
{
    using Type = std::vector<TokenData>;

    std::string archiveMagic;
    uint32_t archiveVersion;
    Type data;

    template<class Archive>
    void save(Archive &ar) const
    {
        ar(archiveMagic, archiveVersion, data);
    }

    template<class Archive>
    void load(Archive &ar)
    {
        ar(archiveMagic, archiveVersion, data);
    }
};

TokenDatabase::Error TokenDatabase::saveTokens()
{
    TokenDataAdapter data;
    data.archiveMagic = TOKEN_ARCHIVE_MAGIC;
    data.archiveVersion = TOKEN_ARCHIVE_VERSION;
    for (auto&& token : TokenStore::i()->tokens())
    {
        TokenData tokenData;
        tokenData.type = token->type();
        tokenData.label = token->label();
        tokenData.secret = mangleTokenSecret(token->secret());
        tokenData.digits = token->digits();
        tokenData.period = token->period();
        tokenData.counter = token->counter();
        tokenData.algorithm = token->algorithm();
        data.data.push_back(tokenData);
    }

    // std::ostreamstream knowns about 0x0 and doesn't interpret them as terminators,
    // which means ::str().size() contains the actual cereal stream size we need to know
    // for the encrypt function.
    std::ostringstream buffer;
    cereal::PortableBinaryOutputArchive archive(buffer);
    archive(data);

    // prepare the stream
    const auto stream = buffer.str();
    buffer.clear();
    std::string encrypted;

    // encrypt the stream
    auto status = encrypt(password, stream, encrypted);
    if (status != Success)
    {
        return status;
    }

    // write the encrypted stream to a file
    status = writeFile(tokenFile, encrypted);
    return status;
}

TokenDatabase::Error TokenDatabase::loadTokens()
{
    // read the encrypted file
    std::string in;
    auto status = readFile(tokenFile, in);
    if (status != Success)
    {
        return status;
    }

    // decrypt the stream
    std::string decrypted;
    status = decrypt(password, in, decrypted);
    if (status != Success)
    {
        in.clear();
        return status;
    }

    // create a string stream for cereal
    std::istringstream buffer(decrypted);

    TokenDataAdapter data;
    cereal::PortableBinaryInputArchive archive(buffer);
    try {
        archive(data);
    } catch (cereal::Exception e) {
        return InvalidTokenFile;
    }

    if (data.archiveMagic != TOKEN_ARCHIVE_MAGIC)
    {
        return InvalidTokenFile;
    }

    TokenStore::i()->clear();

    for (auto&& t : data.data)
    {
        auto unmangledSecret = unmangleTokenSecret(t.secret);
        t.secret.clear();

        switch (t.type)
        {
            case OTPToken::TOTP:
                TokenStore::i()->addTokenUnsafe(std::make_shared<TOTPToken>(TOTPToken(t.label, unmangledSecret, t.digits, t.period, t.counter, t.algorithm)));
                break;
            case OTPToken::HOTP:
                TokenStore::i()->addTokenUnsafe(std::make_shared<HOTPToken>(HOTPToken(t.label, unmangledSecret, t.digits, t.period, t.counter, t.algorithm)));
                break;
            case OTPToken::Steam:
                TokenStore::i()->addTokenUnsafe(std::make_shared<SteamToken>(SteamToken(t.label, unmangledSecret, t.digits, t.period, t.counter, t.algorithm)));
                break;
            case OTPToken::Authy:
                TokenStore::i()->addTokenUnsafe(std::make_shared<AuthyToken>(AuthyToken(t.label, unmangledSecret, t.digits, t.period, t.counter, t.algorithm)));
                break;

            case OTPToken::None:
                break;
        }

        unmangledSecret.clear();
    }

    data.data.clear();

    return Success;
}

const OTPToken::SecretType TokenDatabase::mangleTokenSecret(const OTPToken::SecretType &secret)
{
    auto mangled = secret;
    std::reverse(mangled.begin(), mangled.end());
    return mangled;
}

const OTPToken::SecretType TokenDatabase::unmangleTokenSecret(const OTPToken::SecretType &secret)
{
    // currently the token secret is only reversed
    return mangleTokenSecret(secret);
}

TokenDatabase::Error TokenDatabase::encrypt(const std::string &password,
                                            const std::string &input_buffer, std::string &out, const int64_t &size)
{
    out.clear();

    try {
        CryptoPP::SecByteBlock key(CryptoPP::AES::MAX_KEYLENGTH + CryptoPP::AES::BLOCKSIZE);
        CryptoPP::HKDF<CryptoPP::SHA256> hkdf;
        hkdf.DeriveKey(key, key.size(),
                       reinterpret_cast<const unsigned char*>(password.data()), password.size(),
                       reinterpret_cast<const unsigned char*>(password.data()), password.size(), nullptr, 0);

        std::string ciphertext;

        CryptoPP::AES::Encryption aesEncryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
        CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, reinterpret_cast<const unsigned char*>(password.data()));

        CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(ciphertext));
        auto input_buffer_size = (size == -1 ? input_buffer.size() : static_cast<std::size_t>(size));
        stfEncryptor.Put(reinterpret_cast<const unsigned char*>(input_buffer.data()), input_buffer_size);
        stfEncryptor.MessageEnd();

        for (auto&& c : ciphertext)
        {
            out.push_back(c);
        }

        return Success;
    } catch (CryptoPP::InvalidCiphertext e) {
        // e.what();
        return InvalidCiphertext;
    } catch (...) {
        return EncryptionFailure;
    }
}

TokenDatabase::Error TokenDatabase::encryptFromFile(const std::string &password,
                                                    const std::string &file, std::string &out)
{
    out.clear();

    auto status = readFile(file, out);
    if (status != Success)
    {
        out.clear();
        return status;
    }

    auto in = out;

    return encrypt(password, in, out);
}

TokenDatabase::Error TokenDatabase::decrypt(const std::string &password,
                                            const std::string &input_buffer, std::string &out, const int64_t &size)
{
    out.clear();

    try {
        CryptoPP::SecByteBlock key(CryptoPP::AES::MAX_KEYLENGTH + CryptoPP::AES::BLOCKSIZE);
        CryptoPP::HKDF<CryptoPP::SHA256> hkdf;
        hkdf.DeriveKey(key, key.size(),
                       reinterpret_cast<const unsigned char*>(password.data()), password.size(),
                       reinterpret_cast<const unsigned char*>(password.data()), password.size(), nullptr, 0);

        std::string decryptedtext;

        CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
        CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, reinterpret_cast<const unsigned char*>(password.data()));

        CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(decryptedtext));
        auto input_buffer_size = (size == -1 ? input_buffer.size() : static_cast<std::size_t>(size));
        stfDecryptor.Put(reinterpret_cast<const unsigned char*>(input_buffer.data()), input_buffer_size);
        stfDecryptor.MessageEnd();

        for (auto&& c : decryptedtext)
        {
            out.push_back(c);
        }

        return Success;
    } catch (CryptoPP::InvalidCiphertext e) {
        // e.what();
        return InvalidCiphertext;
    } catch (...) {
        return DecryptionFailure;
    }
}

TokenDatabase::Error TokenDatabase::decryptFromFile(const std::string &password,
                                                    const std::string &file, std::string &out)
{
    out.clear();

    std::string buffer;
    auto status = readFile(file, buffer);
    if (status != Success)
    {
        return status;
    }

    return decrypt(password, buffer, out);
}

TokenDatabase::Error TokenDatabase::readFile(const std::string &file, std::string &out)
{
    std::string buffer;

    try {
        std::ifstream stream(file, std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
        std::streamsize stream_size = stream.tellg();
        stream.seekg(0, std::ios::beg);

        // reserve memory for file
        buffer.resize(stream_size);

        // read file into buffer
        if (!stream.read(buffer.data(), stream_size))
        {
            stream.close();
            buffer.clear();
            return FileReadFailure;
        }

        stream.close();
    } catch (...) {
        return FileReadFailure;
    }

    if (buffer.empty())
    {
        return FileEmpty;
    }

    out = buffer;

    return Success;
}

TokenDatabase::Error TokenDatabase::writeFile(const std::string &location, const std::string &buffer)
{
    try {
        std::ofstream stream(location, std::ios_base::out | std::ios_base::binary);
        if (!stream.write(buffer.data(), buffer.size()))
        {
            stream.close();
            return FileWriteFailure;
        }

        stream.close();
    } catch (...) {
        return FileWriteFailure;
    }

    return Success;
}
