#ifndef OTPTOKEN_HPP
#define OTPTOKEN_HPP

#include <string>
#include <cstdint>

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

    // Token limits
    static const int min_digits;
    static const int max_digits;
    static const int min_period;
    static const int max_period;
    static const int min_counter;
    static const int max_counter;

    OTPToken();
    OTPToken(const OTPToken *other);
    OTPToken(const Label &label);
    virtual ~OTPToken();

    enum TokenType {
        None = 0,

        TOTP = 1,
        HOTP = 2,
        Steam = 3,
        Authy = 4, // TOTP w/ 7 digits and 10s period
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

    // token type
    inline const TokenType &type() const
    { return _type; }
    // token name (type)
    inline const std::string &typeString() const
    { return _typeName; }

    // display name (label)
    inline void setLabel(const Label &label)
    { this->_label = label; }
    inline const Label &label() const
    { return _label; }

    // custom user icon (GUI only)
    inline void setIcon(const Icon &icon)
    { this->_icon = icon; }
    const Icon &icon() const
    { return _icon; }

    // base-32 encoded secret
    inline void setSecret(const SecretType &secret)
    { this->_secret = secret; }
    inline const SecretType &secret() const
    { return _secret; }

    // digit length
    inline void setDigitLength(const DigitType &digits)
    {
        switch (_type)
        {
            case TOTP:
            case HOTP:
            case Authy:
                this->_digits = digits;
                break;
        }
    }
    inline const DigitType &digits() const
    { return _digits; }

    // token validity period
    inline void setPeriod(const PeriodType &period)
    {
        switch (_type)
        {
            case TOTP:
            case Authy:
                this->_period = period;
                break;
        }
    }
    inline const PeriodType &period() const
    { return _period; }

    // token counter
    inline void setCounter(const CounterType &counter)
    {
        switch (_type)
        {
            case HOTP:
                this->_counter = counter;
                break;
        }
    }
    inline const CounterType &counter() const
    { return _counter; }

    // token algorithm
    inline void setAlgorithm(const ShaAlgorithm &algo)
    {
        switch (_type)
        {
            case TOTP:
            case HOTP:
                this->_algorithm = algo;
                break;
        }
    }
    void setAlgorithm(const std::string &algo);
    const ShaAlgorithm &algorithm() const
    { return _algorithm; }
    const std::string algorithmString() const;

#ifdef OTPGEN_DEBUG
    const std::string debug() const;
#endif

    // verify token validity
    bool valid() const;

    // token generation method, implemented in sub classes
    virtual const TokenString generateToken(Error *error = nullptr) const = 0;

protected:
    int sha_enum_to_gcrypt() const;

    // returns true when secret empty
    static bool check_empty(const TokenString &secret, Error *error = nullptr);

    TokenType _type = None;
    std::string _typeName = "None";

    Label _label = Label();
    Icon _icon = Icon();

    SecretType _secret = SecretType();
    DigitType _digits = 0U;
    PeriodType _period = 0U;
    CounterType _counter = 0U;
    ShaAlgorithm _algorithm = Invalid;

public:
    friend struct TokenData;
    friend class TokenStore;

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
        this->setDigitLength(digits);
        this->setPeriod(period);
        this->setCounter(counter);
        this->setAlgorithm(algorithm);
    }
};

#endif // OTPTOKEN_HPP
