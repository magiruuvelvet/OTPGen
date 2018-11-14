#ifndef OTPTOKEN_HPP
#define OTPTOKEN_HPP

#include <iostream>
#include <string>
#include <vector>
#include <cinttypes>

enum class OTPGenErrorCode;

class OTPToken
{
public:
    using TokenType = std::uint8_t;
    using TokenString = std::string;
    using TokenSecret = std::string;
    using Label = std::string;
    using Icon = std::vector<unsigned char>;
    using DigitType = std::uint8_t;
    using PeriodType = std::uint32_t;
    using CounterType = std::uint32_t;
    using ShaAlgorithm = std::uint8_t;

    // sqlite database mapping
    using sqliteShortID = std::int8_t;
    using sqliteLongID = std::int64_t;

    using sqliteSortOrder = sqliteLongID;
    using sqliteTypesID = sqliteShortID;
    using sqliteAlgorithmsID = sqliteShortID;
    using sqliteTokenID = sqliteLongID;

    // token type mapping from database
    enum {
        None  = 0,
        TOTP  = 1,
        HOTP  = 2,
        Steam = 3,
    };

    // sha algorithm mapping from database
    enum {
        Invalid = 0,
        SHA1    = 1,
        SHA256  = 2,
        SHA512  = 3,
    };

public:
    /**
     * construct empty (invalid) token
     */
    OTPToken();

    /**
     * construct empty (invalid) token of given type
     */
    OTPToken(const TokenType &type);

    /**
     * construct a (valid) token with all the required parameters
     */
    OTPToken(const TokenType &type,
             const Label &label,
             const Icon &icon,
             const TokenSecret &secret,
             const DigitType &digits,
             const PeriodType &period,
             const CounterType &counter,
             const ShaAlgorithm &algorithm);

    /**
     * construct a (valid) token and initialize it with default
     * values depending on the given type
     */
    OTPToken(const TokenType &type,
             const Label &label,
             const Icon &icon,
             const TokenSecret &secret);

    /**
     * construct an invalid token of the given type and a label
     */
    OTPToken(const TokenType &type,
             const Label &label);

    /**
     * copy constructor
     */
    OTPToken(const OTPToken &other);

    /**
     * destroy token object
     */
    ~OTPToken();

    // Steam tokens are stored in base-64 (RFC 3548, RFC 4648) on the device,
    // but the library libcotp only supports base-32 (RFC 4648) input.
    // For convenience let the user input the base-64 token and convert it
    // automatically to the correct format.
    bool importBase64Secret(const std::string &base64_str);

    // Static wrapper function for the above method.
    // Converts the base-64 Steam token to base-32 and returns it.
    // On error an empty string is returned.
    static const TokenString convertBase64Secret(const std::string &base64_str);

    // Type
    inline void setType(const TokenType &type)
    { this->_type = type; }
    inline const TokenType &type() const
    { return this->_type; }
    const std::string typeName() const;

    // Label
    inline void setLabel(const Label &label)
    { this->_label = label; }
    inline const Label &label() const
    { return this->_label; }

    // Icon
    // images are stored as std::strings for easier management
    inline void setIcon(const Icon &icon)
    { this->_icon = icon; }
    inline void setIcon(const unsigned char *icon, const std::size_t &size)
    { this->_icon = Icon(icon, icon + size); }
    inline const Icon &icon() const
    { return this->_icon; }
    inline const unsigned char *iconBuffer() const
    { return this->_icon.data(); }
    inline std::size_t iconBufferSize() const
    { return this->_icon.size(); }

    // Secret
    inline void setSecret(const TokenSecret &secret)
    { this->_secret = secret; }
    inline const TokenSecret &secret() const
    { return this->_secret; }

    // Digits
    inline void setDigitLength(const DigitType &digits)
    { this->_digits = digits; }
    inline const DigitType &digitLength() const
    { return this->_digits; }

    // Period
    inline void setPeriod(const PeriodType &period)
    { this->_period = period; }
    inline const PeriodType &period() const
    { return this->_period; }

    // Counter
    inline void setCounter(const CounterType &counter)
    { this->_counter = counter; }
    inline const CounterType &counter() const
    { return this->_counter; }
    inline void increaseCounter()
    { ++this->_counter; }
    inline void decreaseCounter()
    { --this->_counter; }

    // Algorithm
    inline void setAlgorithm(const ShaAlgorithm &algorithm)
    { this->_algorithm = algorithm; }
    inline const ShaAlgorithm &algorithm() const
    { return this->_algorithm; }
    void setAlgorithm(const std::string &algorithm_name);
    const std::string algorithmName() const;

    // ID
    inline const sqliteTokenID &id() const
    { return this->_id; }

    /**
     * get defaults for the given type
     */
    static DigitType defaultDigitLength(const TokenType &type);
    static PeriodType defaultPeriod(const TokenType &type);
    static ShaAlgorithm defaultAlgorithm(const TokenType &type);

    /**
     * get defaults based on the current type
     */
    inline DigitType defaultDigitLength() const
    { return defaultDigitLength(this->_type); }
    inline PeriodType defaultPeriod() const
    { return defaultPeriod(this->_type); }
    inline ShaAlgorithm defaultAlgorithm() const
    { return defaultAlgorithm(this->_type); }

    /**
     * get limits for the given type or the limits of what the library can produce on None
     */
    static DigitType minDigitLength(const TokenType &type = None);
    static DigitType maxDigitLength(const TokenType &type = None);
    static PeriodType minPeriod(const TokenType &type = None);
    static PeriodType maxPeriod(const TokenType &type = None);
    static CounterType minCounter(const TokenType &type = None);
    static CounterType maxCounter(const TokenType &type = None);

    /**
     * get limits based on the current type
     */
    inline DigitType minDigitLength() const
    { return minDigitLength(this->_type); }
    inline DigitType maxDigitLength() const
    { return maxDigitLength(this->_type); }
    inline PeriodType minPeriod() const
    { return minPeriod(this->_type); }
    inline PeriodType maxPeriod() const
    { return maxPeriod(this->_type); }
    inline CounterType minCounter() const
    { return minCounter(this->_type); }
    inline CounterType maxCounter() const
    { return maxCounter(this->_type); }

    /**
     * checks if this token object generates a non-empty token
     */
    bool isValid() const;

    /**
     * tries to generate a one-time password token
     */
    const TokenString generateToken(OTPGenErrorCode *error = nullptr) const;

    /**
     * calculates the remaining token validity from the current system time
     */
    std::uint64_t remainingTokenValidity() const;

    /**
     * equality check
     */
    inline bool operator== (const OTPToken &other) const
    {
        return (
            this->_type == other._type &&
            this->_label == other._label &&
            this->_icon == other._icon &&
            this->_secret == other._secret &&
            this->_digits == other._digits &&
            this->_period == other._period &&
            this->_counter == other._counter &&
            this->_algorithm == other._algorithm
        );
    }
    inline bool operator!= (const OTPToken &other) const
    { return !this->operator== (other); }

    // checks if the token is stored in the database
    // when the token was received using one of the SELECT queries, the ID can NOT be 0
    inline bool isInDatabase()
    { return this->_id != 0; }

private:
    friend class TokenDatabase;

    TokenType _type = 0U;
    Label _label;
    Icon _icon;
    TokenSecret _secret;
    DigitType _digits = 0U;
    PeriodType _period = 0U;
    CounterType _counter = 0U;
    ShaAlgorithm _algorithm = 0U;

    sqliteTokenID _id = 0U;

    static bool validateSecret(const TokenSecret &secret, OTPGenErrorCode *error);
};

inline std::ostream &operator<< (std::ostream &out, const OTPToken &token)
{
    out << "OTPToken{"
        << "type=" << token.typeName() << ", "
        << "label=\"" << token.label() << "\", "
        << "icon=" << +token.iconBufferSize() << "B, "
        << "digits=" << +token.digitLength() << ", "
        << "period=" << +token.period() << ", "
        << "counter=" << +token.counter() << ", "
        << "algorithm=" << token.algorithmName()
        << "}";
    return out;
}

#endif // OTPTOKEN_HPP
