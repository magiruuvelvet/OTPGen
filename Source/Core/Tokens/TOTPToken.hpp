#ifndef TOTPTOKEN_HPP
#define TOTPTOKEN_HPP

#include "OTPToken.hpp"

class TOTPToken : public OTPToken
{
public:
    TOTPToken();
    TOTPToken(const Label &label);

    static const DigitType DEFAULT_DIGIT_LENGTH;
    static const PeriodType DEFAULT_PERIOD;
    static const ShaAlgorithm DEFAULT_ALGORITHM;

    // token counter
    inline void setCounter(const CounterType &)
    { this->_counter = 0U; }

    const TokenString generateToken(OTPGenErrorCode *error = nullptr) const override;

public:
    friend struct TokenData;
    friend class TokenStore;

    std::shared_ptr<OTPToken> clone() const override;

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
        this->setDigitLength(digits);
        this->setPeriod(period);
        this->setCounter(counter);
        this->setAlgorithm(algorithm);
    }
};

#endif // TOTPTOKEN_HPP
