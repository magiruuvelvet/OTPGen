#ifndef AUTHYTOKEN_HPP
#define AUTHYTOKEN_HPP

#include "TOTPToken.hpp"

class AuthyToken final : public TOTPToken
{
public:
    AuthyToken();
    AuthyToken(const Label &label);

public:
    friend struct TokenData;
    friend class TokenStore;

    AuthyToken(const Label &label,
               const Icon &icon,
               const SecretType &secret,
               const DigitType &digits,
               const PeriodType &period,
               const CounterType &counter,
               const ShaAlgorithm &algorithm)
        : AuthyToken()
    {
        _label = label;
        _icon = icon;
        _secret = secret;
        _digits = digits;
        _period = period;
        _counter = counter;
        _algorithm = algorithm;
    }
};

#endif // AUTHYTOKEN_HPP
