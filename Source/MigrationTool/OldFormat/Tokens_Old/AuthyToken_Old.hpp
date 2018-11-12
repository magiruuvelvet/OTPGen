#ifndef AUTHYTOKEN_OLD_HPP
#define AUTHYTOKEN_OLD_HPP

#include "TOTPToken_Old.hpp"

class AuthyToken_Old : public TOTPToken_Old
{
public:
    AuthyToken_Old();
    AuthyToken_Old(const Label &label);

    static const DigitType DEFAULT_DIGIT_LENGTH;
    static const PeriodType DEFAULT_PERIOD;
    static const ShaAlgorithm DEFAULT_ALGORITHM;

    // token algorithm
    inline void setAlgorithm(const ShaAlgorithm &)
    { this->_algorithm = SHA1; }
    void setAlgorithm(const std::string &)
    { this->_algorithm = SHA1; }

public:
    friend struct TokenData;
    friend class TokenStore_Old;

    std::shared_ptr<OTPToken_Old> clone() const override;

    AuthyToken_Old(
               const Label &label,
               const Icon &icon,
               const SecretType &secret,
               const DigitType &digits,
               const PeriodType &period,
               const CounterType &counter,
               const ShaAlgorithm &algorithm)
        : AuthyToken_Old()
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

#endif // AUTHYTOKEN_OLD_HPP
