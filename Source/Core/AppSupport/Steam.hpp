#ifndef STEAM_HPP
#define STEAM_HPP

#include <Core/Tokens/SteamToken.hpp>

namespace AppSupport {

class Steam
{
    Steam() = delete;

public:
    static bool importFromSteamGuard(const std::string &file, SteamToken &target);
};

}

#endif // STEAM_HPP
