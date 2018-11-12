#ifndef TOKENDATABASE_OLD_HPP
#define TOKENDATABASE_OLD_HPP

#include <string>
#include <vector>

#include "Tokens_Old/OTPToken_Old.hpp"

#ifdef OTPGEN_DEBUG
#include <iostream>
#endif

#include "AppSupport.hpp"

class TokenDatabase_Old
{
    TokenDatabase_Old() = delete;

    // for readFile()
    friend class AppSupport::andOTP;
    friend class AppSupport::Authy;
    friend class AppSupport::Steam;

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

    static const std::string getErrorMessage(const Error &error);

    static Error saveTokens();
    static Error loadTokens();

    static bool setPassword(const std::string &password);
    static bool setTokenFile(const std::string &file);

    static Error changePassword(const std::string &newPassword);

private:
    static const OTPToken_Old::SecretType mangleTokenSecret(const OTPToken_Old::SecretType &secret);
    static const OTPToken_Old::SecretType unmangleTokenSecret(const OTPToken_Old::SecretType &secret);

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

#endif // TOKENDATABASE_OLD_HPP
