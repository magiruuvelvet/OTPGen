#ifndef OTPTOKEN_HPP
#define OTPTOKEN_HPP

#include <string>
#include <cstdint>

// forward declarations for friends
namespace Import {
    class andOTP;
    class Authy;
    class Steam;
}

class OTPToken
{
public:
    using TokenString = std::string;
    using SecretType = std::string;
    using Label = std::string;
    using Icon = std::string;
    using DigitType = std::uint8_t;
    using PeriodType = std::uint32_t;
    using CounterType = std::uint32_t;

    OTPToken();
    OTPToken(const Label &label);
    virtual ~OTPToken();

    enum TokenType {
        None = 0,

        TOTP,
        HOTP,
        Steam,
        Authy, // TOTP w/ 7 digits and 10s period
    };

    enum ShaAlgorithm {
        Invalid = 0,

        SHA1    = 1,
        SHA256  = 2,
        SHA512  = 3,
    };

    enum Error {
        VALID = 0,
        GCRYPT_VERSION_MISMATCH = 1,
        INVALID_B32_INPUT       = 2,
        INVALID_ALGO            = 3,
        INVALID_OTP             = 4,
        INVALID_DIGITS          = 5,
        INVALID_PERIOD          = 6,
    };

    const TokenType &type() const;   // token type
    const std::string &name() const; // token name (type)

    Label &label();                  // display name (label)
    Icon &icon();                    // custom user icon (GUI only)

    SecretType &secret();            // base-32 encoded secret
    DigitType &digits();             // digit length
    PeriodType &period();            // token validity period
    CounterType &counter();          // token counter
    ShaAlgorithm &algorithm();       // token algorithm

    inline const Label &getLabel() const
    { return _label; }

    inline const DigitType &getDigits() const
    { return _digits; }

    inline const PeriodType &getPeriod() const
    { return _period; }

    void setAlgorithmFromString(const std::string &algo);

#ifdef OTPGEN_DEBUG
    const std::string debug() const;
#endif

    // verify token validity
    bool valid() const;

    // token generation method, implemented in sub classes
    virtual const TokenString generateToken(Error *error = nullptr) const = 0;

protected:
    int sha_enum_to_gcrypt() const;
    const std::string sha_enum_to_str() const;

    // returns true when secret empty
    static bool check_empty(const TokenString &secret, Error *error = nullptr);

    TokenType _type = None;
    std::string _name = "None";

    Label _label = Label();
    Icon _icon = Icon();

    SecretType _secret = SecretType();
    DigitType _digits = 0U;
    PeriodType _period = 0U;
    CounterType _counter = 0U;
    ShaAlgorithm _algorithm = Invalid;

private:
    friend struct TokenData;
    friend class TokenDatabase;
    friend class TokenEditor;

    friend class Import::andOTP;
    friend class Import::Authy;
    friend class Import::Steam;

    OTPToken(const Label &label,
             const Icon &icon,
             const SecretType &secret,
             const DigitType &digits,
             const PeriodType &period,
             const CounterType &counter,
             const ShaAlgorithm &algorithm)
        : OTPToken()
    {
        _label = label;
        _icon = icon;
        _secret = secret;
        _digits = digits;
        _period = period;
        _counter = counter;
        _algorithm = algorithm;
    }
};

#endif // OTPTOKEN_HPP
