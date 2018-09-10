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

    // compute steam token at current time
    static const OTPToken::TokenString computeSteam(const OTPToken::SecretType &base32_secret,
                                                    OTPGenErrorCode *error = nullptr);

    // compute steam token at a given time
    static const OTPToken::TokenString computeSteam(const std::time_t &time,
                                                    const OTPToken::SecretType &base32_secret,
                                                    OTPGenErrorCode *error = nullptr);
};

#endif // OTPGEN_HPP
