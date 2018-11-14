#include "OTPToken.hpp"
#include "OTPGen.hpp"

#include "TokenDatabase.hpp"

#include <cryptopp/base64.h>
#include <cryptopp/base32.h>
#include <cryptopp/filters.h>

#include <algorithm>
#include <numeric>

OTPToken::OTPToken()
{
}

OTPToken::OTPToken(const TokenType &type)
    : OTPToken()
{
    this->_type = type;
    this->_digits = defaultDigitLength(type);
    this->_period = defaultPeriod(type);
    this->_algorithm = defaultAlgorithm(type);
}

OTPToken::OTPToken(const TokenType &type,
                   const Label &label,
                   const Icon &icon,
                   const TokenSecret &secret,
                   const DigitType &digits,
                   const PeriodType &period,
                   const CounterType &counter,
                   const ShaAlgorithm &algorithm)
{
    this->_type = type;
    this->_label = label;
    this->_icon = icon;
    this->_secret = secret;
    this->_digits = digits;
    this->_period = period;
    this->_counter = counter;
    this->_algorithm = algorithm;
}

OTPToken::OTPToken(const TokenType &type,
                   const Label &label,
                   const Icon &icon,
                   const TokenSecret &secret)
    : OTPToken(type)
{
    this->_label = label;
    this->_icon = icon;
    this->_secret = secret;
}

OTPToken::OTPToken(const TokenType &type,
                   const Label &label)
    : OTPToken(type)
{
    this->_label = label;
}

OTPToken::OTPToken(const OTPToken &other)
    : OTPToken()
{
    this->_type = other._type;
    this->_label = other._label;
    this->_icon = other._icon;
    this->_secret = other._secret;
    this->_digits = other._digits;
    this->_period = other._period;
    this->_counter = other._counter;
    this->_algorithm = other._algorithm;

    this->_id = other._id;
}

OTPToken::~OTPToken()
{
    this->_type = None;
    this->_label.clear();
    this->_icon.clear();
    this->_secret.clear();
    this->_digits = 0U;
    this->_period = 0U;
    this->_counter = 0U;
    this->_algorithm = Invalid;

    this->_id = 0U;
}

bool OTPToken::importBase64Secret(const std::string &base64_str)
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

const OTPToken::TokenString OTPToken::convertBase64Secret(const std::string &base64_str)
{
    OTPToken token;
    token.importBase64Secret(base64_str);
    return token.secret();
}

const std::string OTPToken::typeName() const
{
    const auto name = TokenDatabase::selectTokenTypeName(static_cast<sqliteTypesID>(this->_type));
    // fallback when database is not connected
    if (name.empty())
    {
        switch (this->_type)
        {
            case None:  return {};
            case TOTP:  return "TOTP";
            case HOTP:  return "HOTP";
            case Steam: return "Steam";
        }
    }
    return name;
}

void OTPToken::setAlgorithm(const std::string &algorithm_name)
{
    std::string algo = algorithm_name;
    std::transform(algo.begin(), algo.end(), algo.begin(), [](unsigned char c) {
        return std::tolower(c);
    });

    if (algo == "sha1" || algo == "sha-1")
    {
        _algorithm = SHA1;
    }
    else if (algo == "sha256" || algo == "sha-256")
    {
        _algorithm = SHA256;
    }
    else if (algo == "sha512" || algo == "sha-512")
    {
        _algorithm = SHA512;
    }
    else
    {
        _algorithm = Invalid;
    }
}

const std::string OTPToken::algorithmName() const
{
    const auto name = TokenDatabase::selectAlgorithmName(static_cast<sqliteAlgorithmsID>(this->_algorithm));
    // fallback when database is not connected
    if (name.empty())
    {
        switch (this->_algorithm)
        {
            case Invalid: return {};
            case SHA1:    return "SHA1";
            case SHA256:  return "SHA256";
            case SHA512:  return "SHA512";
        }
    }
    return name;
}

OTPToken::DigitType OTPToken::defaultDigitLength(const TokenType &type)
{
    switch (type)
    {
        case None:  return 0U;
        case TOTP:  return 6U;
        case HOTP:  return 6U;
        case Steam: return 5U;
    }

    return 0U;
}

OTPToken::PeriodType OTPToken::defaultPeriod(const TokenType &type)
{
    switch (type)
    {
        case None:  return 0U;
        case TOTP:  return 30U;
        case HOTP:  return 0U;
        case Steam: return 30U;
    }

    return 0U;
}

OTPToken::ShaAlgorithm OTPToken::defaultAlgorithm(const TokenType &type)
{
    switch (type)
    {
        case None:  return Invalid;
        case TOTP:  return SHA1;
        case HOTP:  return SHA1;
        case Steam: return SHA1;
    }

    return Invalid;
}

OTPToken::DigitType OTPToken::minDigitLength(const TokenType &type)
{
    switch (type)
    {
        case None:  return OTPGen::minDigitLength();
        case TOTP:  return 3U;
        case HOTP:  return 3U;
        case Steam: return 5U;
    }

    return 0U;
}

OTPToken::DigitType OTPToken::maxDigitLength(const TokenType &type)
{
    switch (type)
    {
        case None:  return OTPGen::maxDigitLength();
        case TOTP:  return 10U;
        case HOTP:  return 10U;
        case Steam: return 5U;
    }

    return 0U;
}

OTPToken::PeriodType OTPToken::minPeriod(const TokenType &type)
{
    switch (type)
    {
        case None:  return OTPGen::minPeriod();
        case TOTP:  return 1U;
        case HOTP:  return 0U;
        case Steam: return 30U;
    }

    return 0U;
}

OTPToken::PeriodType OTPToken::maxPeriod(const TokenType &type)
{
    switch (type)
    {
        case None:  return OTPGen::maxPeriod();
        case TOTP:  return 120U;
        case HOTP:  return 0U;
        case Steam: return 30U;
    }

    return 0U;
}

OTPToken::CounterType OTPToken::minCounter(const TokenType &type)
{
    switch (type)
    {
        case None:  return OTPGen::minCounter();
        case TOTP:  return 0U;
        case HOTP:  return 0U;
        case Steam: return 0U;
    }

    return 0U;
}

OTPToken::CounterType OTPToken::maxCounter(const TokenType &type)
{
    switch (type)
    {
        case None:  return OTPGen::maxCounter();
        case TOTP:  return 0U;
        case HOTP:  return std::numeric_limits<CounterType>::max();
        case Steam: return 0U;
    }

    return 0U;
}

bool OTPToken::isValid() const
{
    if (_label.empty())
    {
        return false;
    }

    auto error = OTPGenErrorCode::Valid;
    const auto &token = this->generateToken(&error);
    return !(token.empty() || error != OTPGenErrorCode::Valid);
}

const OTPToken::TokenString OTPToken::generateToken(OTPGenErrorCode *error) const
{
    if (error)
    {
        (*error) = OTPGenErrorCode::Valid;
    }

    // invalid type, can't compute a token
    if (_type == None)
    {
        if (error)
        {
            (*error) = OTPGenErrorCode::InvalidType;
        }
        return TokenString();
    }

    // secret must not be empty
    if (!validateSecret(_secret, error))
    {
        return TokenString();
    }

    // store compute error
    auto err = OTPGenErrorCode::Valid;

    // store generated token
    TokenString token;

    // generate token based on type
    if (_type == TOTP)
    {
        token = OTPGen::computeTOTP(_secret, _digits, _period, _algorithm, &err);
    }
    else if (_type == HOTP)
    {
        token = OTPGen::computeHOTP(_secret, _counter, _digits, _algorithm, &err);
    }
    else if (_type == Steam)
    {
        token = OTPGen::computeSteam(_secret, &err);
    }
    else
    {
        if (error)
        {
            (*error) = OTPGenErrorCode::InvalidType;
        }
        return TokenString();
    }

    // check if we got a token
    if (!token.empty() && err == OTPGenErrorCode::Valid)
    {
        return token;
    }

    // error during generation occurred
    if (error)
    {
        (*error) = err;
    }

    return TokenString();
}

std::uint64_t OTPToken::remainingTokenValidity() const
{
    if (_period == 0U)
    {
        return 0U;
    }

    // get remaining seconds since last minute
    auto now = std::time(nullptr);
    auto local = std::localtime(&now);

    // calculate token validity with 1 second update threshold
    auto sec_expired = local->tm_sec;
    auto token_validity = ( static_cast<int>(_period) - sec_expired );
    if (token_validity < 0)
    {
        token_validity = ( static_cast<int>(_period) - (sec_expired % static_cast<int>(_period)) ) + 1;
    }
    else
    {
        token_validity++;
    }

    return static_cast<std::uint64_t>(token_validity);
}

bool OTPToken::validateSecret(const TokenSecret &secret, OTPGenErrorCode *error)
{
    if (secret.empty())
    {
        if (error)
        {
            (*error) = OTPGenErrorCode::InvalidBase32Input;
        }
        return false;
    }
    return true;
}
