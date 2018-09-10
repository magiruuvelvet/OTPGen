#ifndef OTPGEN_HPP
#define OTPGEN_HPP

/**
 * Code is based on https://github.com/paolostivanin/libcotp
 *
 * Significant changes are:
 *  -> use std::string instead of C char pointers
 *  -> automatic memory management, no manual std::free() needed
 *  -> use crypto++ instead of gcrypt
 *  -> no external dependency (libbaseencode) required
 *  -> error handling is optimal (error=nullptr)
 *  -> code is C++11 compatible
 *
 */

#include <string>
#include <ctime>

#include "OTPGenErrorCodes.hpp"

#include <Tokens/OTPToken.hpp>

class OTPGen
{
    OTPGen() = delete;

public:

    // compute totp at current time
    static const OTPToken::TokenString computeTOTP(const OTPToken::SecretType &base32_secret,
                                                   const OTPToken::DigitType &digits,
                                                   const OTPToken::PeriodType &period,
                                                   const OTPToken::ShaAlgorithm &sha_algo,
                                                   OTPGenErrorCode *error = nullptr);

    // compute totp at a given time
    static const OTPToken::TokenString computeTOTP(const std::time_t &time,
                                                   const OTPToken::SecretType &base32_secret,
                                                   const OTPToken::DigitType &digits,
                                                   const OTPToken::PeriodType &period,
                                                   const OTPToken::ShaAlgorithm &sha_algo,
                                                   OTPGenErrorCode *error = nullptr);

    // compute hotp
    static const OTPToken::TokenString computeHOTP(const OTPToken::SecretType &base32_secret,
                                                   const OTPToken::CounterType &counter,
                                                   const OTPToken::DigitType &digits,
                                                   const OTPToken::ShaAlgorithm &sha_algo,
                                                   OTPGenErrorCode *error = nullptr);

private:
    static const OTPToken::TokenString hotp_helper(const OTPToken::SecretType &base32_secret,
                                                   const std::time_t &counter,
                                                   const OTPToken::DigitType &digits,
                                                   const OTPToken::ShaAlgorithm &sha_algo,
                                                   OTPGenErrorCode *error = nullptr);

public:

    // compute steam token at current time
    static const OTPToken::TokenString computeSteam(const OTPToken::SecretType &base32_secret,
                                                    OTPGenErrorCode *error = nullptr);

    // compute steam token at a given time
    static const OTPToken::TokenString computeSteam(const std::time_t &time,
                                                    const OTPToken::SecretType &base32_secret,
                                                    OTPGenErrorCode *error = nullptr);

private:
    static const std::string computeHMAC(const std::string &key, long C, const OTPToken::ShaAlgorithm &algo);
    static const std::string base32_rfc4648_decode(const std::string &key);
    static const std::string normalizeSecret(const std::string &secret);
    static const std::string finalize(const OTPToken::DigitType &digits_length, int tk);

    static int truncate(const std::string &hmac,
                        const OTPToken::DigitType &digits_length,
                        const OTPToken::ShaAlgorithm algo);

    static bool checkPeriod(const OTPToken::PeriodType &period);
    static bool checkOTPLength(const OTPToken::DigitType &digits_length);
    static bool checkAlgo(const OTPToken::ShaAlgorithm &algo);
};

#endif // OTPGEN_HPP
