#include "HOTPToken_Old.hpp"

const OTPToken_Old::DigitType HOTPToken_Old::DEFAULT_DIGIT_LENGTH = 6U;
const OTPToken_Old::ShaAlgorithm HOTPToken_Old::DEFAULT_ALGORITHM = OTPToken_Old::SHA1;

HOTPToken_Old::HOTPToken_Old()
{
    // set HOTP defaults
    _type = HOTP;
    _typeName = "HOTP";

    _digits = DEFAULT_DIGIT_LENGTH;
    _period = 0U; // period is not used in HOTP
    _counter = 0U;
    _algorithm = DEFAULT_ALGORITHM;
}

HOTPToken_Old::HOTPToken_Old(const Label &label)
    : HOTPToken_Old()
{
    _label = label;
}

std::shared_ptr<OTPToken_Old> HOTPToken_Old::clone() const
{
    return std::make_shared<HOTPToken_Old>(_label, _icon, _secret, _digits, _period, _counter, _algorithm);
}
