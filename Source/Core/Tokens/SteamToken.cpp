#include "SteamToken.hpp"

#include "Internal/libcotpsupport.hpp"

#include <cryptopp/base64.h>
#include <cryptopp/base32.h>
#include <cryptopp/filters.h>

const OTPToken::DigitType SteamToken::DEFAULT_DIGIT_LENGTH = 5U;
const OTPToken::PeriodType SteamToken::DEFAULT_PERIOD = 30U;
const OTPToken::ShaAlgorithm SteamToken::DEFAULT_ALGORITHM = OTPToken::SHA1;

SteamToken::SteamToken()
{
    // set Steam TOTP defaults
    _type = Steam;
    _typeName = "Steam";

    _digits = DEFAULT_DIGIT_LENGTH; // digit count is always 5
    _period = DEFAULT_PERIOD; // period is always 30 seconds
    _counter = 0U; // counter is not used in Steam TOTP
    _algorithm = DEFAULT_ALGORITHM; // Steam TOTP can only have a single algorithm
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

    try {

        // create an RFC 4648 base-32 encoder
        // crypto++ uses DUDE by default which isn't TOTP compatible
        auto encoder = new CryptoPP::Base32Encoder();
        static const CryptoPP::byte ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
        static const CryptoPP::AlgorithmParameters params = CryptoPP::MakeParameters(
                                                            CryptoPP::Name::EncodingLookupArray(),
                                                            static_cast<const CryptoPP::byte*>(ALPHABET));
        encoder->IsolatedInitialize(params);

        encoder->Attach(new CryptoPP::StringSink(_secret));

        // decode and reencode base-64 data into base-32
        CryptoPP::StringSource src(base64_str, true,
            new CryptoPP::Base64Decoder(encoder));

    } catch (...) {
        _secret.clear();
        return false;
    }

    if (_secret.empty())
    {
        return false;
    }

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
                                DEFAULT_DIGIT_LENGTH,
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
