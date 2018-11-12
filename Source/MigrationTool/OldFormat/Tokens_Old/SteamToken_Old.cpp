#include "SteamToken_Old.hpp"

#include <cryptopp/base64.h>
#include <cryptopp/base32.h>
#include <cryptopp/filters.h>

const OTPToken_Old::DigitType SteamToken_Old::DEFAULT_DIGIT_LENGTH = 5U;
const OTPToken_Old::PeriodType SteamToken_Old::DEFAULT_PERIOD = 30U;
const OTPToken_Old::ShaAlgorithm SteamToken_Old::DEFAULT_ALGORITHM = OTPToken_Old::SHA1;

SteamToken_Old::SteamToken_Old()
{
    // set Steam TOTP defaults
    _type = Steam;
    _typeName = "Steam";

    _digits = DEFAULT_DIGIT_LENGTH; // digit count is always 5
    _period = DEFAULT_PERIOD; // period is always 30 seconds
    _counter = 0U; // counter is not used in Steam TOTP
    _algorithm = DEFAULT_ALGORITHM; // Steam TOTP can only have a single algorithm
}

SteamToken_Old::SteamToken_Old(const Label &label)
    : SteamToken_Old()
{
    _label = label;
}

bool SteamToken_Old::importBase64Secret(const std::string &base64_str)
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

const SteamToken_Old::TokenString SteamToken_Old::convertBase64Secret(const std::string &base64_str)
{
    SteamToken_Old token;
    token.importBase64Secret(base64_str);
    return token.secret();
}

std::shared_ptr<OTPToken_Old> SteamToken_Old::clone() const
{
    return std::make_shared<SteamToken_Old>(_label, _icon, _secret, _digits, _period, _counter, _algorithm);
}
