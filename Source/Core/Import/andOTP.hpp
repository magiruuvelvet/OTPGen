#ifndef ANDOTP_HPP
#define ANDOTP_HPP

#include <Core/TOTPToken.hpp>

#include <vector>

namespace Import {

class andOTP
{
    andOTP() = delete;

public:
    enum Type {
        PlainText,
        Encrypted,
    };

    static bool importTOTP(const std::string &file, std::vector<TOTPToken> &target, const Type &type = PlainText, const std::string &password = std::string());
};

}

#endif // ANDOTP_HPP
