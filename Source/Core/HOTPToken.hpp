#ifndef HOTPTOKEN_HPP
#define HOTPTOKEN_HPP

#include "OTPToken.hpp"

class HOTPToken final : public OTPToken
{
public:
    HOTPToken();
    HOTPToken(const Label &label);

    const TokenString generateToken(Error *error = nullptr) const override;

private:
    friend struct TokenData;
    friend class TokenStore;
    friend class TokenDatabase;
    friend class TokenEditor;

    friend class Import::andOTP;
    friend class Import::Authy;
    friend class Import::Steam;

    HOTPToken(const Label &label,
              const Icon &icon,
              const SecretType &secret,
              const DigitType &digits,
              const PeriodType &period,
              const CounterType &counter,
              const ShaAlgorithm &algorithm)
        : HOTPToken()
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

#endif // HOTPTOKEN_HPP
