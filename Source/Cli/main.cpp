#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>

#include <AppConfig.hpp>
#include <CommandLineOperation.hpp>

#include <TokenDatabase.hpp>
#include <TokenStore.hpp>

#include "StdinEchoMode.hpp"

#include <sago/platform_folders.h>

int main(int argc, char **argv)
{
    std::printf("%s CLI\n\n", cfg::Name.c_str());

#ifdef OTPGEN_DEBUG
    TokenDatabase::setPassword("pwd123");
#else
    std::string password;
    std::cout << "Enter your token database password: ";

    SetStdinEcho(false);
    std::cin >> password;
    SetStdinEcho(true);

    if (!TokenDatabase::setPassword(password))
    {
        std::cerr << "Password may not be empty!" << std::endl;
        return 1;
    }

    password.clear();

    std::cout << std::endl;
#endif

    const auto config_home = sago::getConfigHome();
    const auto app_cfg = config_home + "/" + cfg::Developer + "/" + cfg::Name;

#ifdef OTPGEN_DEBUG
    TokenDatabase::setTokenFile(app_cfg + "/database.debug");
#else
    TokenDatabase::setTokenFile(app_cfg + "/database");
#endif

    auto status = TokenDatabase::loadTokens();
    if (status == TokenDatabase::FileReadFailure)
    {
        status = TokenDatabase::saveTokens();
        if (status != TokenDatabase::Success)
        {
            std::cerr << "Unable to initialize the token database!" << std::endl;
            return 1;
        }
    }
    if (status != TokenDatabase::Success)
    {
        std::cerr << "Unable to load the token database! Is the password correct?" << std::endl;
        return 1;
    }

    // run command line operation if any
    const std::vector<std::string> args(argv, argv + argc);
    exec_commandline_operation(args);

    return 0;
}
