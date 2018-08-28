#include "TOTPToken.hpp"

#include "Internal/libcotpsupport.hpp"

TOTPToken::TOTPToken()
{
    // set TOTP defaults
    _type = TOTP;
    _typeName = "TOTP";

    _digits = 6U;
    _period = 30U;
    _counter = 0U; // counter is not used in TOTP
    _algorithm = SHA1;
}

TOTPToken::TOTPToken(const Label &label)
    : TOTPToken()
{
    _label = label;
}

const TOTPToken::TokenString TOTPToken::generateToken(Error *error) const
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

    auto token = get_totp(_secret.c_str(),
                          static_cast<int>(_digits),
                          static_cast<int>(_period),
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
