#ifndef AUTHY_HPP
#define AUTHY_HPP

#include <Core/TOTPToken.hpp>
#include <Core/AuthyToken.hpp>

#include <vector>

namespace Import {

class Authy
{
    Authy() = delete;

public:
    static bool importTOTP(const std::string &file, std::vector<TOTPToken> &target);
    static bool importNative(const std::string &file, std::vector<AuthyToken> &target);
};

}

#endif // AUTHY_HPP
