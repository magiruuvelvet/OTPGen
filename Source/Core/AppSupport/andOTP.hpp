#ifndef ANDOTP_HPP
#define ANDOTP_HPP

#include <Tokens.hpp>

#include <vector>

namespace AppSupport {

class andOTP
{
    andOTP() = delete;

    // andOTP cipher constants
    static const uint8_t ANDOTP_IV_SIZE;
    static const uint8_t ANDOTP_TAG_SIZE;

public:
    enum Type {
        PlainText,
        Encrypted,
    };

    static bool importTokens(const std::string &file, std::vector<OTPToken*> &target, const Type &type = PlainText, const std::string &password = std::string());
    static bool exportTokens(const std::string &target, const std::vector<OTPToken*> &tokens);

private:
    static const std::string sha256_password(const std::string &password);
    static bool decrypt(const std::string &password, const std::string &buffer, std::string &decrypted);
};

}

#endif // ANDOTP_HPP
