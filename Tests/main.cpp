#include <bandit/bandit.h>

#include <iostream>

using namespace snowhouse;
using namespace bandit;

#include "blank-test.hpp"

#ifdef OTPGEN_WITH_QR_CODES
#include "qr-code-test.hpp"
#endif

#include "otpauth-tests.hpp"
#include "steam-base-test.hpp"

int main(int argc, char **argv)
{
    std::cout << "OTPGen Unit Tests" << std::endl << std::endl;

#ifdef OTPGEN_WITH_QR_CODES
    std::cout << "Has QR Codes?            true" << std::endl;
#else
    std::cout << "Has QR Codes?            false" << std::endl;
#endif

    std::cout << std::endl;

    // run all registered tests
    // new tests are registered with the go_bandit macro
    return bandit::run(argc, argv);
}
