#ifndef TOTPTOKEN_HPP
#define TOTPTOKEN_HPP

#include "OTPToken.hpp"

class TOTPToken : public OTPToken
{
public:
    TOTPToken();
    TOTPToken(const Label &label);

    // token counter
    inline void setCounter(const CounterType &)
    { this->_counter = 0U; }

    const TokenString generateToken(Error *error = nullptr) const override;

public:
    friend struct TokenData;
    friend class TokenStore;

    TOTPToken(const Label &label,
              const Icon &icon,
              const SecretType &secret,
              const DigitType &digits,
              const PeriodType &period,
              const CounterType &counter,
              const ShaAlgorithm &algorithm)
        : TOTPToken()
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

#endif // TOTPTOKEN_HPP