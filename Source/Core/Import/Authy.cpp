#include "Authy.hpp"

namespace Import {

bool Authy::importTOTP(const std::string &file, std::vector<TOTPToken> &target)
{
    return false;
}

bool Authy::importNative(const std::string &file, std::vector<AuthyToken> &target)
{
    return false;
}

}
