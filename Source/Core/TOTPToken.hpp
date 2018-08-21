#ifndef TOTPTOKEN_HPP
#define TOTPTOKEN_HPP

#include "OTPToken.hpp"

class TOTPToken : public OTPToken
{
public:
    TOTPToken();
    TOTPToken(const Label &label);

    const TokenString generateToken(Error *error = nullptr) const override;

private:
    friend struct TokenData;
    friend class TokenDatabase;
    friend class TokenEditor;

    friend class Import::andOTP;
    friend class Import::Authy;
    friend class Import::Steam;

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
