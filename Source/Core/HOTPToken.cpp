#include "HOTPToken.hpp"

extern "C" {
    #include <libcotp/cotp.h>
}

// undefine conflicting macros
#undef SHA1
#undef SHA256
#undef SHA512

HOTPToken::HOTPToken()
{
    // set HOTP defaults
    _type = HOTP;
    _name = "HOTP";

    _digits = 6U;
    _period = 0U; // period is not used in HOTP
    _counter = 0U;
    _algorithm = SHA1;
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
