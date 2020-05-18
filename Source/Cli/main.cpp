#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>

#include <AppConfig.hpp>
#include <Signals.hpp>
#include <CommandLineOperation.hpp>

#include <TokenDatabase.hpp>

#include <StdinEchoMode.hpp>

#include <sago/platform_folders.h>

#include <filesystem>
#include <system_error>

#if !defined(OS_WINDOWS)
// gracefully terminate application
__attribute__((noreturn))
static void graceful_terminate(int signal)
{
    // close database connection handle and cleanup
    TokenDatabase::closeDatabase();

    // restore original signal handler
    std::signal(signal, signals.at(signal));

    // raise original signal
    std::raise(signal);

    // avoid a warning about __noreturn__
    // or exit if ever reached here
    std::exit(128 + signal);
}
#endif

int main(int argc, char **argv)
{
#if !defined(OS_WINDOWS)
    // register signals
    for (auto&& sig : {SIGINT, SIGTERM, SIGQUIT, SIGHUP,
                       SIGSEGV, SIGILL, SIGABRT})
    {
        // register current signal and store original handler function
        auto orig_handler = std::signal(sig, &graceful_terminate);
        // push original handler function into the signal map
        signals.insert({sig, orig_handler});
    }
#endif

    std::printf("%s CLI\n\n", cfg::Name.c_str());

    const auto config_home = sago::getConfigHome();
    const auto app_cfg = config_home + "/" + cfg::Developer + "/" + cfg::Name;

    std::error_code fs_error;
    if (!std::filesystem::exists(app_cfg, fs_error))
    {
        std::cout << "[info] first start! creating config directory: " << app_cfg << std::endl << std::endl;

        auto fs_res = std::filesystem::create_directories(app_cfg, fs_error);
        if (!fs_res)
        {
            std::fprintf(stderr, "Failed to create directory: %s\n std::filesystem: %s\n", app_cfg.c_str(), fs_error.message().c_str());
            return 1;
        }
    }

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

#ifdef OTPGEN_DEBUG
    TokenDatabase::setTokenDatabase(app_cfg + "/tokens.db.debug");
#else
    TokenDatabase::setTokenDatabase(app_cfg + "/tokens.db");
#endif

    auto status = TokenDatabase::loadTokens();
    if (status == TokenDatabase::FileReadFailure)
    {
        status = TokenDatabase::initializeTokens();
        if (status != TokenDatabase::Success)
        {
            std::cerr << "Unable to initialize the token database!" << std::endl;
            std::cerr << "Detailed error: " << TokenDatabase::getErrorMessage(status) << std::endl;
            return 1;
        }
    }
    if (status != TokenDatabase::Success)
    {
        std::cerr << "Unable to load the token database! Is the password correct?" << std::endl;
        std::cerr << "Detailed error: " << TokenDatabase::getErrorMessage(status) << std::endl;
        return 1;
    }

    // run command line operation if any
    // FIXME: refactor how command line options are parsed and handled
    //        <remove this function>
    const std::vector<std::string> args(argv, argv + argc);
    exec_commandline_operation(args);

    // TODO: cli application code goes here

    TokenDatabase::closeDatabase();
    return 0;
}
