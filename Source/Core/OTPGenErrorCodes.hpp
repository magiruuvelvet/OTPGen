#ifndef OTPGENERRORCODES_HPP
#define OTPGENERRORCODES_HPP

enum class OTPGenErrorCode {
    Valid = 0,
    InvalidBase32Input,
    InvalidAlgorithm,
    InvalidOTP,
    InvalidDigits,
    InvalidPeriod,
};

#endif // OTPGENERRORCODES_HPP
