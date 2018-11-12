#ifndef STEAM_HPP
#define STEAM_HPP

#include <OTPToken.hpp>

namespace AppSupport {

class Steam
{
    Steam() = delete;

public:
    static bool importFromSteamGuard(const std::string &file, OTPToken &target);
};

}

#endif // STEAM_HPP
