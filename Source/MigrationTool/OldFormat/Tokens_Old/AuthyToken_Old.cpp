#include "AuthyToken_Old.hpp"

const OTPToken_Old::DigitType AuthyToken_Old::DEFAULT_DIGIT_LENGTH = 7U;
const OTPToken_Old::PeriodType AuthyToken_Old::DEFAULT_PERIOD = 10U;
const OTPToken_Old::ShaAlgorithm AuthyToken_Old::DEFAULT_ALGORITHM = OTPToken_Old::SHA1;

AuthyToken_Old::AuthyToken_Old()
{
    // set Authy TOTP defaults
    _type = Authy;
    _typeName = "Authy";

    _digits = DEFAULT_DIGIT_LENGTH;
    _period = DEFAULT_PERIOD;
    _counter = 0U; // counter is not used in TOTP
    _algorithm = DEFAULT_ALGORITHM;
}

AuthyToken_Old::AuthyToken_Old(const Label &label)
    : AuthyToken_Old()
{
    _label = label;
}

std::shared_ptr<OTPToken_Old> AuthyToken_Old::clone() const
{
    return std::make_shared<AuthyToken_Old>(_label, _icon, _secret, _digits, _period, _counter, _algorithm);
}
