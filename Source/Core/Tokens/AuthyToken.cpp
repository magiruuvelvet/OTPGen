#include "AuthyToken.hpp"

const OTPToken::DigitType AuthyToken::DEFAULT_DIGIT_LENGTH = 7U;
const OTPToken::PeriodType AuthyToken::DEFAULT_PERIOD = 10U;
const OTPToken::ShaAlgorithm AuthyToken::DEFAULT_ALGORITHM = OTPToken::SHA1;

AuthyToken::AuthyToken()
{
    // set Authy TOTP defaults
    _type = Authy;
    _typeName = "Authy";

    _digits = DEFAULT_DIGIT_LENGTH;
    _period = DEFAULT_PERIOD;
    _counter = 0U; // counter is not used in TOTP
    _algorithm = DEFAULT_ALGORITHM;
}

AuthyToken::AuthyToken(const Label &label)
    : AuthyToken()
{
    _label = label;
}

std::shared_ptr<OTPToken> AuthyToken::clone() const
{
    return std::make_shared<AuthyToken>(_label, _icon, _secret, _digits, _period, _counter, _algorithm);
}
