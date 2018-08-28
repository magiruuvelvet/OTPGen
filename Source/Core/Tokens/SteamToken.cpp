#include "SteamToken.hpp"

#include "Internal/libcotpsupport.hpp"

SteamToken::SteamToken()
{
    // set Steam TOTP defaults
    _type = Steam;
    _typeName = "Steam";

    _digits = 5U; // digit count is always 5
    _period = 30U; // period is always 30 seconds
    _counter = 0U; // counter is not used in Steam TOTP
    _algorithm = SHA1; // Steam TOTP can only have a single algorithm
}

SteamToken::SteamToken(const Label &label)
    : SteamToken()
{
    _label = label;
}

bool SteamToken::importBase64Secret(const std::string &base64_str)
{
    // input can't be empty
    if (base64_str.empty())
    {
        return false;
    }

    // decode base-64 data
    baseencode_error_t err = baseencode_error_t::SUCCESS;
    auto data = base64_decode(base64_str.c_str(), base64_str.size(), &err);

    if (err != baseencode_error_t::SUCCESS)
    {
        // data is NULL here
        return false;
    }

    // encode data to base-32
    auto str = base32_encode(data, strlen( (const char*) data ), &err);
    std::free(data);

    if (err != baseencode_error_t::SUCCESS)
    {
        // str is NULL HERE
        return false;
    }

    _secret = std::string(str);
    std::free(str);

    return true;
}

const SteamToken::TokenString SteamToken::convertBase64Secret(const std::string &base64_str)
{
    SteamToken token;
    token.importBase64Secret(base64_str);
    return token.secret();
}

const SteamToken::TokenString SteamToken::generateToken(Error *error) const
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

    // Steam secret must be in base-32
    auto token = get_steam_totp(_secret.c_str(),
                                30,
                                &cotp_err);

    if (token && cotp_err == cotp_error_t::VALID)
    {
        const TokenString str(token);
        std::free(token);
        return str;
    }

    if (error)
    {
        (*error) = static_cast<Error>(cotp_err);
    }

    return TokenString();
}
