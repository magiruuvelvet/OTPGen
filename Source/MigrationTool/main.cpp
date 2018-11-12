#include <iostream>

#include <StdinEchoMode.hpp>

///
/// OLD FORMAT HEADERS
///
#include <OldFormat/TokenDatabase_Old.hpp>
#include <OldFormat/TokenStore_Old.hpp>

///
/// NEW FORMAT HEADERS
///
#include <TokenDatabase.hpp>

void print_usage()
{
    std::cout << "Usage: otpgen-migratedb inputfile (outputfile=tokens.db)" << std::endl;
}

int main(int argc, char **argv)
{
    std::cout << "OTPGen Migration Tool" << std::endl;

    // check amount of command line arguments
    if (argc < 2 || argc > 3)
    {
        print_usage();
        return 1;
    }

    // check if to use default outputfile "tokens.db"
    bool default_ofile = false;
    if (argc == 2)
    {
        default_ofile = true;
    }


    TokenDatabase_Old::setTokenFile(argv[1]);

    std::string password;
    std::cout << "Enter your token database password: ";

    SetStdinEcho(false);
    std::cin >> password;
    SetStdinEcho(true);

    if (!TokenDatabase_Old::setPassword(password))
    {
        std::cerr << "Password may not be empty!" << std::endl;
        return 1;
    }

    password.clear();

    std::cout << std::endl;

    auto status = TokenDatabase_Old::loadTokens();
    if (status != TokenDatabase_Old::Success)
    {
        std::cerr << TokenDatabase_Old::getErrorMessage(status) << std::endl;
        return 1;
    }

//  LD_LIBRARY_PATH=../lib ./otpgen-migratedb

    return 0;
}
