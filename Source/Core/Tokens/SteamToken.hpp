#ifndef STEAMTOKEN_HPP
#define STEAMTOKEN_HPP

#include "OTPToken.hpp"

class SteamToken final : public OTPToken
{
public:
    SteamToken();
    SteamToken(const Label &label);

    static const DigitType DEFAULT_DIGIT_LENGTH;
    static const PeriodType DEFAULT_PERIOD;
    static const ShaAlgorithm DEFAULT_ALGORITHM;

    // token algorithm
    inline void setAlgorithm(const ShaAlgorithm &)
    { this->_algorithm = SHA1; }
    void setAlgorithm(const std::string &)
    { this->_algorithm = SHA1; }

    // digit length
    inline void setDigitLength(const DigitType &)
    { this->_digits = 5U; }

    // token validity period
    inline void setPeriod(const PeriodType &)
    { this->_period = 30U; }

    // token counter
    inline void setCounter(const CounterType &)
    { this->_counter = 0U; }

    // Steam tokens are stored in base-64 (RFC 3548, RFC 4648) on the device,
    // but the library libcotp only supports base-32 (RFC 4648) input.
    // For convenience let the user input the base-64 token and convert it
    // automatically to the correct format.
    bool importBase64Secret(const std::string &base64_str);

    // Static wrapper function for the above method.
    // Converts the base-64 Steam token to base-32 and returns it.
    // On error an empty string is returned.
    static const TokenString convertBase64Secret(const std::string &base64_str);

    const TokenString generateToken(Error *error = nullptr) const override;

public:
    friend struct TokenData;
    friend class TokenStore;

    SteamToken(const Label &label,
               const Icon &icon,
               const SecretType &secret,
               const DigitType &digits = SteamToken::DEFAULT_DIGIT_LENGTH,
               const PeriodType &period = SteamToken::DEFAULT_PERIOD,
               const CounterType &counter = 0U,
               const ShaAlgorithm &algorithm = SteamToken::DEFAULT_ALGORITHM)
        : SteamToken()
    {
        _label = label;
        _icon = icon;
        _secret = secret;
        this->setDigitLength(digits);
        this->setPeriod(period);
        this->setCounter(counter);
        this->setAlgorithm(algorithm);
    }
};

#endif // STEAMTOKEN_HPP
