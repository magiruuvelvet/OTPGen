#ifndef TOKENDATABASE_HPP
#define TOKENDATABASE_HPP

#include <string>
#include <vector>

#include "OTPToken.hpp"

// forward declarations for friends
namespace Import {
    class andOTP;
    class Authy;
    class Steam;
}

class TokenDatabase
{
    TokenDatabase() = delete;

    // for readFile()
    friend class Import::andOTP;
    friend class Import::Authy;
    friend class Import::Steam;

    static std::string password;
    static std::string tokenFile;

public:
    enum Error {
        Success = 0,

        FileReadFailure,   // unable to read file or file not found or directory given
        FileWriteFailure,  // unable to write file
        FileEmpty,         // file is empty
        InvalidTokenFile,  // token file is invalid

        EncryptionFailure, // unknown encryption failure
        DecryptionFailure, // unknown decryption failure
        InvalidCiphertext, // wrong password or cipher, or corrupt input buffer
        PasswordEmpty,     // password is empty
        PasswordHashFailure, // failed to hash password

        UnknownFailure,    // unknown or unhandled error
    };

    static Error saveTokens();
    static Error loadTokens();

    static bool setPassword(const std::string &password);
    static bool setTokenFile(const std::string &file);

    static Error changePassword(const std::string &newPassword);

private:
    static const OTPToken::SecretType mangleTokenSecret(const OTPToken::SecretType &secret);
    static const OTPToken::SecretType unmangleTokenSecret(const OTPToken::SecretType &secret);

    static Error encrypt(const std::string &password,
                         const std::string &input_buffer, std::string &out, const int64_t &size = -1);
    static Error encryptFromFile(const std::string &password,
                                 const std::string &file, std::string &out);

    static Error decrypt(const std::string &password,
                         const std::string &input_buffer, std::string &out, const int64_t &size = -1);
    static Error decryptFromFile(const std::string &password,
                                 const std::string &file, std::string &out);

    static Error readFile(const std::string &file, std::string &out);
    static Error writeFile(const std::string &location, const std::string &buffer);
};

#endif // TOKENDATABASE_HPP
