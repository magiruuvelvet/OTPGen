#include <iostream>

// library must be installed already for this test to compile and work

#include <otpgen/Tokens.hpp>

// minimal libotpgen test
// library is not yet ready for external usage
//
// missing bits:
//  OTPGenConfig.cmake
//  Config.hpp

int main(void)
{
    OTPToken token;
    token.setLabel("test");
    token.setSecret("aabbcc");

    std::cout << token.label() << ": " << token.generateToken() << std::endl;

    return 0;
}
