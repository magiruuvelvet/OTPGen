#include "TokenDatabase_Old.hpp"
#include "TokenStore_Old.hpp"

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
    static const uint32_t TOKEN_ARCHIVE_VERSION = 0x02;
}

std::string TokenDatabase_Old::password;
std::string TokenDatabase_Old::tokenFile;

const std::string TokenDatabase_Old::getErrorMessage(const Error &error)
{
    switch(error)
    {
        case Success: return "";

        case FileReadFailure:     return "Unable to read file.";
        case FileWriteFailure:    return "Unable to write file.";
        case FileEmpty:           return "File is empty.";
        case InvalidTokenFile:    return "Not a valid token database.";

        case EncryptionFailure:   return "Failed to encrypt data.";
        case DecryptionFailure:   return "Failed to decrypt data.";
        case InvalidCiphertext:   return "Failed to decrypt data. Either the password is incorrect or the file is corrupt.";
        case PasswordEmpty:       return "Password is empty.";
        case PasswordHashFailure: return "Failed to hash password.";

        case UnknownFailure:      return "An unknown error occurred!";
    }

    return "";
}

bool TokenDatabase_Old::setPassword(const std::string &password)
{
    if (password.empty())
        return false;

    // remove old password
    TokenDatabase_Old::password.clear();

    // don't use smart pointers here, already managed/deleted by crypto++ itself
    CryptoPP::SHA256 hash;
    CryptoPP::StringSource src(password, true,
        new CryptoPP::HashFilter(hash,
            new CryptoPP::Base64Encoder(
                new CryptoPP::StringSink(TokenDatabase_Old::password))));

    return true;
}

bool TokenDatabase_Old::setTokenFile(const std::string &file)
{
    if (file.empty())
        return false;

    tokenFile = file;
    return true;
}

TokenDatabase_Old::Error TokenDatabase_Old::changePassword(const std::string &newPassword)
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
    OTPToken_Old::TokenType type;
    OTPToken_Old::Label label;
    OTPToken_Old::Icon icon;
    OTPToken_Old::SecretType secret;
    OTPToken_Old::DigitType digits;
    OTPToken_Old::PeriodType period;
    OTPToken_Old::CounterType counter;
    OTPToken_Old::ShaAlgorithm algorithm;

    template<class Archive>
    void serialize(Archive &ar)
    {
        ar(type, label, icon, secret, digits, period, counter, algorithm);
    }
};

// Container to store the entire token store in a single file.
template<class TokenDataVersion = TokenData>
struct TokenDataAdapter
{
    using Type = std::vector<TokenDataVersion>;

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

TokenDatabase_Old::Error TokenDatabase_Old::saveTokens()
{
    TokenDataAdapter<> data;
    data.archiveMagic = TOKEN_ARCHIVE_MAGIC;
    data.archiveVersion = TOKEN_ARCHIVE_VERSION;
    for (auto&& token : TokenStore_Old::i()->tokens())
    {
        TokenData tokenData;
        tokenData.type = token->type();
        tokenData.label = token->label();
        tokenData.icon = token->icon();
        tokenData.secret = mangleTokenSecret(token->secret());
        tokenData.digits = token->digits();
        tokenData.period = token->period();
        tokenData.counter = token->counter();
        tokenData.algorithm = token->algorithm();
#ifdef OTPGEN_DEBUG
        std::cout << "Saving " << tokenData.label << std::endl;
#endif
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

TokenDatabase_Old::Error TokenDatabase_Old::loadTokens()
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

#ifdef OTPGEN_DEBUG
//    std::cout << "=== BINARY ARCHIVE BEGIN ===" << std::endl;
//    std::cout << decrypted << std::endl;
//    std::cout << "=== BINARY ARCHIVE END ===" << std::endl;
#endif

    TokenDataAdapter<> data;
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

    TokenStore_Old::i()->clear();

    for (auto&& t : data.data)
    {
#ifdef OTPGEN_DEBUG
        std::cout << "Loading " << t.label << std::endl;
#endif

        auto unmangledSecret = unmangleTokenSecret(t.secret);
        t.secret.clear();

        switch (t.type)
        {
            case OTPToken_Old::TOTP:
                TokenStore_Old::i()->addTokenUnsafe(std::make_shared<TOTPToken_Old>(t.label, t.icon, unmangledSecret, t.digits, t.period, t.counter, t.algorithm));
                break;
            case OTPToken_Old::HOTP:
                TokenStore_Old::i()->addTokenUnsafe(std::make_shared<HOTPToken_Old>(t.label, t.icon, unmangledSecret, t.digits, t.period, t.counter, t.algorithm));
                break;
            case OTPToken_Old::Steam:
                TokenStore_Old::i()->addTokenUnsafe(std::make_shared<SteamToken_Old>(t.label, t.icon, unmangledSecret, t.digits, t.period, t.counter, t.algorithm));
                break;
            case OTPToken_Old::Authy:
                TokenStore_Old::i()->addTokenUnsafe(std::make_shared<AuthyToken_Old>(t.label, t.icon, unmangledSecret, t.digits, t.period, t.counter, t.algorithm));
                break;
        }

        unmangledSecret.clear();
    }

    data.data.clear();

    return Success;
}

const OTPToken_Old::SecretType TokenDatabase_Old::mangleTokenSecret(const OTPToken_Old::SecretType &secret)
{
    auto mangled = secret;
    std::reverse(mangled.begin(), mangled.end());
    return mangled;
}

const OTPToken_Old::SecretType TokenDatabase_Old::unmangleTokenSecret(const OTPToken_Old::SecretType &secret)
{
    // currently the token secret is only reversed
    return mangleTokenSecret(secret);
}

TokenDatabase_Old::Error TokenDatabase_Old::encrypt(const std::string &password,
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

TokenDatabase_Old::Error TokenDatabase_Old::encryptFromFile(const std::string &password,
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

TokenDatabase_Old::Error TokenDatabase_Old::decrypt(const std::string &password,
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

TokenDatabase_Old::Error TokenDatabase_Old::decryptFromFile(const std::string &password,
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

TokenDatabase_Old::Error TokenDatabase_Old::readFile(const std::string &file, std::string &out)
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

TokenDatabase_Old::Error TokenDatabase_Old::writeFile(const std::string &location, const std::string &buffer)
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
