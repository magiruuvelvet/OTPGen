#include "AuthyToken.hpp"

AuthyToken::AuthyToken()
{
    // set Authy TOTP defaults
    _type = Authy;
    _name = "Authy";

    _digits = 7U;
    _period = 10U;
    _counter = 0U; // counter is not used in TOTP
    _algorithm = SHA1;
}

AuthyToken::AuthyToken(const Label &label)
    : AuthyToken()
{
    _label = label;
}
