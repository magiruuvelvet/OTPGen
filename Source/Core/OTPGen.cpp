#include "OTPGen.hpp"

#include <cryptopp/filters.h>
#include <cryptopp/base32.h>
#include <cryptopp/base64.h>
#include <cryptopp/hmac.h>
#include <cryptopp/sha.h>

#include <Tokens/SteamToken.hpp>

namespace {
    static const constexpr auto SHA1_DIGEST_SIZE = 20;
    static const constexpr auto SHA256_DIGEST_SIZE = 32;
    static const constexpr auto SHA512_DIGEST_SIZE = 64;

    static const constexpr std::uint64_t DIGITS_POWER[] = {
        1,
        10,
        100,
        1000,
        10000,
        100000,
        1000000,
        10000000,
        100000000,
        1000000000,
        10000000000,
    };
}

// compute totp at current time
const OTPToken::TokenString OTPGen::computeTOTP(const OTPToken::SecretType &base32_secret,
                                                const OTPToken::DigitType &digits,
                                                const OTPToken::PeriodType &period,
                                                const OTPToken::ShaAlgorithm &sha_algo,
                                                OTPGenErrorCode *error)
{
    return computeTOTP(time(nullptr), base32_secret, digits, period, sha_algo, error);
}

// compute totp at a given time
const OTPToken::TokenString OTPGen::computeTOTP(const std::time_t &time,
                                                const OTPToken::SecretType &base32_secret,
                                                const OTPToken::DigitType &digits,
                                                const OTPToken::PeriodType &period,
                                                const OTPToken::ShaAlgorithm &sha_algo,
                                                OTPGenErrorCode *error)
{
    if (!checkOTPLength(digits))
    {
        if (error) (*error) = OTPGenErrorCode::InvalidOTP;
        return {};
    }

    if (!checkPeriod(period))
    {
        if (error) (*error) = OTPGenErrorCode::InvalidPeriod;
    }

    auto timestamp = time / period;

    // use hotp with the timestamp as counter to compute a totp token
    return hotp_helper(base32_secret, timestamp, digits, sha_algo, error);
}

// compute hotp
const OTPToken::TokenString OTPGen::computeHOTP(const OTPToken::SecretType &base32_secret,
                                                const OTPToken::CounterType &counter,
                                                const OTPToken::DigitType &digits,
                                                const OTPToken::ShaAlgorithm &sha_algo,
                                                OTPGenErrorCode *error)
{
    if (!checkAlgo(sha_algo))
    {
        if (error) (*error) = OTPGenErrorCode::InvalidAlgorithm;
        return {};
    }

    if (!checkOTPLength(digits))
    {
        if (error) (*error) = OTPGenErrorCode::InvalidOTP;
        return {};
    }

    return hotp_helper(base32_secret, counter, digits, sha_algo, error);
}

const OTPToken::TokenString OTPGen::hotp_helper(const OTPToken::SecretType &base32_secret,
                                                const std::time_t &counter,
                                                const OTPToken::DigitType &digits,
                                                const OTPToken::ShaAlgorithm &sha_algo,
                                                OTPGenErrorCode *error)
{
    const auto hmac = computeHMAC(base32_secret, counter, sha_algo);
    if (hmac.empty())
    {
        if (error) (*error) = OTPGenErrorCode::InvalidBase32Input;
        return {};
    }

    auto tk = truncate(hmac, digits, sha_algo);
    return finalize(digits, tk);
}

// compute steam token at current time
const OTPToken::TokenString OTPGen::computeSteam(const OTPToken::SecretType &base32_secret,
                                                 OTPGenErrorCode *error)
{
    return computeSteam(time(nullptr), base32_secret, error);
}

// compute steam token at a given time
const OTPToken::TokenString OTPGen::computeSteam(const std::time_t &time,
                                                 const OTPToken::SecretType &base32_secret,
                                                 OTPGenErrorCode *error)
{
    auto timestamp = time / SteamToken::DEFAULT_PERIOD;

    const auto hmac = computeHMAC(base32_secret, timestamp, OTPToken::SHA1);
    if (hmac.empty())
    {
        if (error) (*error) = OTPGenErrorCode::InvalidBase32Input;
        return {};
    }

    int offset = (hmac[SHA1_DIGEST_SIZE-1] & 0x0f);

    // Starting from the offset, take the successive 4 bytes while stripping the topmost bit to prevent it being handled as a signed integer
    int bin_code = ((hmac[offset] & 0x7f) << 24) | ((hmac[offset + 1] & 0xff) << 16) | ((hmac[offset + 2] & 0xff) << 8) | ((hmac[offset + 3] & 0xff));

    static const std::string steam_alphabet = "23456789BCDFGHJKMNPQRTVWXY";

    char code[6];
    for (auto i = 0; i < 5; i++)
    {
        int mod = bin_code % steam_alphabet.size();
        bin_code = bin_code / steam_alphabet.size();
        code[i] = steam_alphabet[mod];
    }
    code[5] = '\0';

    std::string codeStr(code, code + strlen(code));
    return codeStr;
}

// template helper function to compute HMAC's of different SHA algorithms
template<class CryptoPPHMacClass>
static inline const std::string compute_hmac_private(const std::string &key, unsigned char value[8])
{
    CryptoPPHMacClass cryptoHmac(reinterpret_cast<const unsigned char*>(key.data()), key.size());

    std::string hmac;
    CryptoPP::StringSource(value, 8, true,
        new CryptoPP::HashFilter(cryptoHmac,
            new CryptoPP::StringSink(hmac)));
    return hmac;
}

const std::string OTPGen::computeHMAC(const std::string &key, long C, const OTPToken::ShaAlgorithm &algo)
{
    // normalize and decode secret
    const auto normalized_key = normalizeSecret(key);
    const auto secret = base32_rfc4648_decode(normalized_key);

    // don't continue on empty secret
    if (secret.empty())
    {
        return {};
    }

    // calculate reverse byte order of C
    unsigned char C_reverse_byte_order[8];
    int j, i;
    for (j = 0, i = 7; j < 8 && i >= 0; j++, i--)
    {
        C_reverse_byte_order[i] = (reinterpret_cast<unsigned char*>(&C))[j];
    }

    // compute the HMAC and store it into a string
    std::string hmac;
    switch (algo)
    {
        case OTPToken::SHA1:   hmac = compute_hmac_private<CryptoPP::HMAC<CryptoPP::SHA1>>(secret, C_reverse_byte_order); break;
        case OTPToken::SHA256: hmac = compute_hmac_private<CryptoPP::HMAC<CryptoPP::SHA256>>(secret, C_reverse_byte_order); break;
        case OTPToken::SHA512: hmac = compute_hmac_private<CryptoPP::HMAC<CryptoPP::SHA512>>(secret, C_reverse_byte_order); break;
    }

    // validate HMAC
    if (hmac.empty())
    {
        return {};
    }

    return hmac;
}

const std::string OTPGen::base32_rfc4648_decode(const std::string &key)
{
    if (key.empty())
    {
        return {};
    }

    // create an RFC 4648 base-32 decoder
    // crypto++ uses DUDE by default which isn't TOTP compatible
    auto decoder = new CryptoPP::Base32Decoder();

    static int lookup[256];
    static const CryptoPP::byte ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    static const auto init = ([&]{ CryptoPP::Base64Decoder::InitializeDecodingLookupArray(lookup, ALPHABET, 32, true); return 0; })(); (void) init;
    static const CryptoPP::AlgorithmParameters params = CryptoPP::MakeParameters(
                                                        CryptoPP::Name::DecodingLookupArray(),
                                                        static_cast<const int*>(lookup));
    decoder->IsolatedInitialize(params);

    // raw pointers are automatically deleted by crypto++
    std::string base32;
    decoder->Attach(new CryptoPP::StringSink(base32));

    // result may be binary (unsigned char)
    CryptoPP::StringSource(key, true, decoder);

    return base32;
}

const std::string OTPGen::normalizeSecret(const std::string &secret)
{
    auto nK = static_cast<char*>(std::calloc(1, secret.size() + 1));

    std::size_t i = 0, j = 0;
    while (secret[i] != '\0')
    {
        if (secret[i] != ' ')
        {
            if (secret[i] >= 'a' && secret[i] <= 'z')
            {
                nK[j++] = static_cast<char>( (secret[i] - 32) );
            }
            else
            {
                nK[j++] = secret[i];
            }
        }
        i++;
    }

    std::string normalized(nK, nK + strlen(nK));
    std::free(nK);
    return normalized;
}

const std::string OTPGen::finalize(const OTPToken::DigitType &digits_length, int tk)
{
    auto token = static_cast<char*>(std::malloc(digits_length + 1));

    int extra_char = digits_length < 10 ? 0 : 1;
    auto fmt = static_cast<char*>(std::calloc(1, 5 + extra_char));
    std::memcpy(fmt, "%.", 2);
    std::snprintf(fmt + 2, 2 + extra_char, "%d", digits_length);
    std::memcpy(fmt + 3 + extra_char, "d", 2);
    std::snprintf(token, digits_length + 1, fmt, tk);
    std::free(fmt);

    std::string tokenStr(token, token + strlen(token));
    std::free(token);
    return tokenStr;
}

int OTPGen::truncate(const std::string &hmac,
                     const OTPToken::DigitType &digits_length,
                     const OTPToken::ShaAlgorithm algo)
{
    // take the lower four bits of the last byte
    int offset = 0;
    switch (algo)
    {
        case OTPToken::SHA1:
            offset = (hmac[SHA1_DIGEST_SIZE-1] & 0x0f);
            break;
        case OTPToken::SHA256:
            offset = (hmac[SHA256_DIGEST_SIZE-1] & 0x0f);
            break;
        case OTPToken::SHA512:
            offset = (hmac[SHA512_DIGEST_SIZE-1] & 0x0f);
            break;
    }

    // Starting from the offset, take the successive 4 bytes while stripping the topmost bit to prevent it being handled as a signed integer
    int bin_code = ((hmac[offset] & 0x7f) << 24) | ((hmac[offset + 1] & 0xff) << 16) | ((hmac[offset + 2] & 0xff) << 8) | ((hmac[offset + 3] & 0xff));

    int token = bin_code % DIGITS_POWER[digits_length];

    return token;
}

bool OTPGen::checkPeriod(const OTPToken::PeriodType &period)
{
    return !(period <= 0 || period > 120);
}

bool OTPGen::checkOTPLength(const OTPToken::DigitType &digits_length)
{
    return !(digits_length < 3 || digits_length > 10);
}

bool OTPGen::checkAlgo(const OTPToken::ShaAlgorithm &algo)
{
    switch (algo)
    {
        case OTPToken::SHA1:
        case OTPToken::SHA256:
        case OTPToken::SHA512:
            return true;
    }

    return false;
}
