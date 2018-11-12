#include "TOTPToken_Old.hpp"

const OTPToken_Old::DigitType TOTPToken_Old::DEFAULT_DIGIT_LENGTH = 6U;
const OTPToken_Old::PeriodType TOTPToken_Old::DEFAULT_PERIOD = 30U;
const OTPToken_Old::ShaAlgorithm TOTPToken_Old::DEFAULT_ALGORITHM = OTPToken_Old::SHA1;

TOTPToken_Old::TOTPToken_Old()
{
    // set TOTP defaults
    _type = TOTP;
    _typeName = "TOTP";

    _digits = DEFAULT_DIGIT_LENGTH;
    _period = DEFAULT_PERIOD;
    _counter = 0U; // counter is not used in TOTP
    _algorithm = DEFAULT_ALGORITHM;
}

TOTPToken_Old::TOTPToken_Old(const Label &label)
    : TOTPToken_Old()
{
    _label = label;
}

std::shared_ptr<OTPToken_Old> TOTPToken_Old::clone() const
{
    return std::make_shared<TOTPToken_Old>(_label, _icon, _secret, _digits, _period, _counter, _algorithm);
}
