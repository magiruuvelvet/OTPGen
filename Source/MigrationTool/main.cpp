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

int load_old(const std::string &path)
{
    if (!TokenDatabase_Old::setTokenFile(path))
    {
        std::cerr << "old: path may not be empty!" << std::endl;
        return 1;
    }

#ifdef OTPGEN_DEBUG
    std::string password = "pwd123";
#else
    std::string password;
    std::cout << "Enter your token database password: ";

    SetStdinEcho(false);
    std::cin >> password;
    SetStdinEcho(true);
#endif

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

    return 0;
}

int init_new(const std::string &path)
{

    if (!TokenDatabase::setTokenDatabase(path))
    {
        std::cerr << "new: path may not be empty!" << std::endl;
        return 1;
    }

#ifdef OTPGEN_DEBUG
    std::string password = "pwd123";
#else
    std::string password;
    std::cout << "Password for the new database: ";

    SetStdinEcho(false);
    std::cin >> password;
    SetStdinEcho(true);
#endif

    if (!TokenDatabase::setPassword(password))
    {
        std::cerr << "Password may not be empty!" << std::endl;
        return 1;
    }

    password.clear();

    std::cout << std::endl;

    auto status = TokenDatabase::initializeTokens();
    if (status != TokenDatabase::Success)
    {
        std::cerr << TokenDatabase::getErrorMessage(status) << std::endl;
        return 1;
    }

    return 0;
}

void do_migration()
{
    for (auto&& token : TokenStore_Old::i()->tokens())
    {
        // type mapping changed
        OTPToken::TokenType new_type;
        switch (token->type())
        {
            case OTPToken_Old::None:  new_type = OTPToken::None; break;
            case OTPToken_Old::TOTP:  new_type = OTPToken::TOTP; break;
            case OTPToken_Old::HOTP:  new_type = OTPToken::HOTP; break;
            case OTPToken_Old::Steam: new_type = OTPToken::Steam; break;
            case OTPToken_Old::Authy: new_type = OTPToken::TOTP; break;
        }

        // algorithm mapping
        OTPToken::ShaAlgorithm new_algo;
        switch (token->algorithm())
        {
            case OTPToken_Old::Invalid: new_algo = OTPToken::Invalid; break;
            case OTPToken_Old::SHA1:    new_algo = OTPToken::SHA1; break;
            case OTPToken_Old::SHA256:  new_algo = OTPToken::SHA256; break;
            case OTPToken_Old::SHA512:  new_algo = OTPToken::SHA512; break;
        }

        // icon format changed
        const auto old_icon = reinterpret_cast<const unsigned char*>(token->icon().data());
        OTPToken::Icon new_icon(old_icon, old_icon + token->icon().size());

        // insert migrated token
        auto status = TokenDatabase::insertToken(OTPToken(
            new_type,
            token->label(),
            new_icon,
            token->secret(),
            token->digits(),
            token->period(),
            token->counter(),
            new_algo
        ));

        // check for errors and inform user about failed/skipped tokens
        if (status != TokenDatabase::Success)
        {
            std::cerr << "failed to insert: " << token->label() << std::endl;
        }
    }

    auto status = TokenDatabase::saveTokens();
    if (status != TokenDatabase::Success)
    {
        std::cerr << TokenDatabase::getErrorMessage(status) << std::endl;
    }
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

    // load old database
    auto ret = load_old(argv[1]);
    if (ret != 0)
    {
        return ret;
    }

    // initialize new database
    ret = init_new(default_ofile ? "./tokens.db" : argv[1]);
    if (ret != 0)
    {
        return ret;
    }

    // migrate
    do_migration();

    // close database
    TokenDatabase::closeDatabase();

    return 0;
}
