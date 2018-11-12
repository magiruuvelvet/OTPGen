#ifndef OTPGENERRORCODES_HPP
#define OTPGENERRORCODES_HPP

enum class OTPGenErrorCode {
    Valid = 0,
    InvalidType,
    InvalidBase32Input,
    InvalidAlgorithm,
    InvalidDigits,
    InvalidPeriod,
};

#endif // OTPGENERRORCODES_HPP
