#include "HOTPToken.hpp"

#include "Internal/libcotpsupport.hpp"

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

const HOTPToken::TokenString HOTPToken::generateToken(Error *error) const
{
    if (error)
    {
        (*error) = VALID;
    }

    // secret must not be empty
    if (check_empty(_secret, error))
    {
        return TokenString();
    }

    cotp_error_t cotp_err = cotp_error_t::VALID;

    auto token = get_hotp(_secret.c_str(),
                          static_cast<int>(_counter),
                          static_cast<int>(_digits),
                          sha_enum_to_gcrypt(),
                          &cotp_err);

    if (token && cotp_err == cotp_error_t::VALID)
    {
        TokenString str(token);
        std::free(token);
        return str;
    }

    if (error)
    {
        (*error) = static_cast<Error>(cotp_err);
    }

    return TokenString();
}
