#ifndef HOTPTOKEN_OLD_HPP
#define HOTPTOKEN_OLD_HPP

#include "OTPToken_Old.hpp"

class HOTPToken_Old : public OTPToken_Old
{
public:
    HOTPToken_Old();
    HOTPToken_Old(const Label &label);

    static const DigitType DEFAULT_DIGIT_LENGTH;
    static const ShaAlgorithm DEFAULT_ALGORITHM;

    // token validity period
    inline void setPeriod(const PeriodType &)
    { this->_period = 0U; }

public:
    friend struct TokenData;
    friend class TokenStore_Old;

    std::shared_ptr<OTPToken_Old> clone() const override;

    HOTPToken_Old(
              const Label &label,
              const Icon &icon,
              const SecretType &secret,
              const DigitType &digits,
              const PeriodType &period,
              const CounterType &counter,
              const ShaAlgorithm &algorithm)
        : HOTPToken_Old()
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

#endif // HOTPTOKEN_OLD_HPP
