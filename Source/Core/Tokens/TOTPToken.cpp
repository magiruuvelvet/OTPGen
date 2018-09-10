#include "TOTPToken.hpp"

#include <OTPGen.hpp>

const OTPToken::DigitType TOTPToken::DEFAULT_DIGIT_LENGTH = 6U;
const OTPToken::PeriodType TOTPToken::DEFAULT_PERIOD = 30U;
const OTPToken::ShaAlgorithm TOTPToken::DEFAULT_ALGORITHM = OTPToken::SHA1;

TOTPToken::TOTPToken()
{
    // set TOTP defaults
    _type = TOTP;
    _typeName = "TOTP";

    _digits = DEFAULT_DIGIT_LENGTH;
    _period = DEFAULT_PERIOD;
    _counter = 0U; // counter is not used in TOTP
    _algorithm = DEFAULT_ALGORITHM;
}

TOTPToken::TOTPToken(const Label &label)
    : TOTPToken()
{
    _label = label;
}

const TOTPToken::TokenString TOTPToken::generateToken(OTPGenErrorCode *error) const
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

    auto token = OTPGen::computeTOTP(_secret,
                                     _digits,
                                     _period,
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
