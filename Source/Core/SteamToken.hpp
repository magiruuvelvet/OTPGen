#ifndef STEAMTOKEN_HPP
#define STEAMTOKEN_HPP

#include "OTPToken.hpp"

class SteamToken final : public OTPToken
{
public:
    SteamToken();
    SteamToken(const Label &label);

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

private:
    friend struct TokenData;
    friend class TokenDatabase;
    friend class TokenEditor;

    friend class Import::andOTP;
    friend class Import::Authy;
    friend class Import::Steam;

    SteamToken(const Label &label,
               const SecretType &secret,
               const DigitType &digits,
               const PeriodType &period,
               const CounterType &counter,
               const ShaAlgorithm &algorithm)
        : SteamToken()
    {
        _label = label;
        _secret = secret;
        _digits = digits;
        _period = period;
        _counter = counter;
        _algorithm = algorithm;
    }
};

#endif // STEAMTOKEN_HPP
