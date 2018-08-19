#ifndef ANDOTP_HPP
#define ANDOTP_HPP

#include <Core/TOTPToken.hpp>

#include <vector>

namespace Import {

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

    static bool importTOTP(const std::string &file, std::vector<TOTPToken> &target, const Type &type = PlainText, const std::string &password = std::string());

private:
    static const std::string sha256_password(const std::string &password);
    static bool decrypt(const std::string &password, const std::string &buffer, std::string &decrypted);
};

}

#endif // ANDOTP_HPP
