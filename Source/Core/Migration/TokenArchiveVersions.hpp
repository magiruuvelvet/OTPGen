#ifndef TOKENARCHIVEVERSIONS_HPP
#define TOKENARCHIVEVERSIONS_HPP

#include <Core/OTPToken.hpp>

struct TokenDataV1
{
    OTPToken::TokenType type;
    OTPToken::Label label;
    OTPToken::SecretType secret;
    OTPToken::DigitType digits;
    OTPToken::PeriodType period;
    OTPToken::CounterType counter;
    OTPToken::ShaAlgorithm algorithm;

    template<class Archive>
    void serialize(Archive &ar)
    {
        ar(type, label, secret, digits, period, counter, algorithm);
    }
};

#endif // TOKENARCHIVEVERSIONS_HPP
