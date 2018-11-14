#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>

#include <TokenDatabase.hpp>

void exec_commandline_operation(const std::vector<std::string> &args)
{
    if (args.size() > 1)
    {
        if (args.at(1) == "--swap")
        {
            if (args.size() != 4)
            {
                std::cerr << "Swap operation requires 2 labels!" << std::endl;
                std::exit(2);
            }

            const auto res = TokenDatabase::swapTokens(args.at(2), args.at(3));
            if (res == TokenDatabase::Success)
            {
                std::printf("Swapped \"%s\" with \"%s\".\n", args.at(2).c_str(), args.at(3).c_str());
                TokenDatabase::saveTokens();
                std::exit(0);
            }
            else
            {
                std::fprintf(stderr, "Swapping of \"%s\" and \"%s\" failed.\n", args.at(2).c_str(), args.at(3).c_str());
                std::fprintf(stderr, "Error: %s\n", TokenDatabase::getErrorMessage(res).c_str());
                std::exit(3);
            }
        }
        else if (args.at(1) == "--move")
        {
            if (args.size() != 4)
            {
                std::cerr << "Swap operation requires 1 label and a new position or a label (below)!" << std::endl;
                std::exit(2);
            }

            unsigned long newPos = 0;
            bool ok = false;
            TokenDatabase::Error res = TokenDatabase::UnknownFailure;
            try {
                newPos = std::stoul(args.at(3));
                ok = true;
            } catch (...) {
                ok = false;
            }
            if (!ok)
            {
                res = TokenDatabase::moveTokenBelow(args.at(2).c_str(), args.at(3).c_str());
            }
            else
            {
                res = TokenDatabase::moveToken(args.at(2).c_str(), newPos);
            }

            if (res == TokenDatabase::Success)
            {
                std::cout << "Move operation successful." << std::endl;
                TokenDatabase::saveTokens();
                std::exit(0);
            }
            else
            {
                std::cerr << "Move operation failed." << std::endl;
                std::cerr << "Error: " << TokenDatabase::getErrorMessage(res) << std::endl;
                std::exit(3);
            }
        }
    }
}
