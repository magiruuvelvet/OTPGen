#include "HOTPToken.hpp"

#include <OTPGen.hpp>

const OTPToken::DigitType HOTPToken::DEFAULT_DIGIT_LENGTH = 6U;
const OTPToken::ShaAlgorithm HOTPToken::DEFAULT_ALGORITHM = OTPToken::SHA1;

HOTPToken::HOTPToken()
{
    // set HOTP defaults
    _type = HOTP;
    _typeName = "HOTP";

    _digits = DEFAULT_DIGIT_LENGTH;
    _period = 0U; // period is not used in HOTP
    _counter = 0U;
    _algorithm = DEFAULT_ALGORITHM;
}

HOTPToken::HOTPToken(const Label &label)
    : HOTPToken()
{
    _label = label;
}

const HOTPToken::TokenString HOTPToken::generateToken(OTPGenErrorCode *error) const
{
    if (error)
    {
        (*error) = OTPGenErrorCode::Valid;
    }

    // secret must not be empty
    if (check_empty(_secret, error))
    {
        return TokenString();
    }

    OTPGenErrorCode err = OTPGenErrorCode::Valid;

    auto token = OTPGen::computeHOTP(_secret,
                                     _counter,
                                     _digits,
                                     _algorithm,
                                     &err);

    if (!token.empty() && err == OTPGenErrorCode::Valid)
    {
        return token;
    }

    if (error)
    {
        (*error) = err;
    }

    return TokenString();
}

std::shared_ptr<OTPToken> HOTPToken::clone() const
{
    return std::make_shared<HOTPToken>(_label, _icon, _secret, _digits, _period, _counter, _algorithm);
}
