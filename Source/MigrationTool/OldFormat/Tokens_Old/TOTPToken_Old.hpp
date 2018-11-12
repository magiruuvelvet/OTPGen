#ifndef TOTPTOKEN_OLD_HPP
#define TOTPTOKEN_OLD_HPP

#include "OTPToken_Old.hpp"

class TOTPToken_Old : public OTPToken_Old
{
public:
    TOTPToken_Old();
    TOTPToken_Old(const Label &label);

    static const DigitType DEFAULT_DIGIT_LENGTH;
    static const PeriodType DEFAULT_PERIOD;
    static const ShaAlgorithm DEFAULT_ALGORITHM;

    // token counter
    inline void setCounter(const CounterType &)
    { this->_counter = 0U; }

public:
    friend struct TokenData;
    friend class TokenStore_Old;

    std::shared_ptr<OTPToken_Old> clone() const override;

    TOTPToken_Old(
              const Label &label,
              const Icon &icon,
              const SecretType &secret,
              const DigitType &digits,
              const PeriodType &period,
              const CounterType &counter,
              const ShaAlgorithm &algorithm)
        : TOTPToken_Old()
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

#endif // TOTPTOKEN_OLD_HPP
