#ifndef OTPAUTHURI_HPP
#define OTPAUTHURI_HPP

#include <map>
#include <string>

/**
 * otpauth URI
 *
 * see: https://github.com/google/google-authenticator/wiki/Key-Uri-Format
 *
 * otpauth://TYPE/LABEL?PARAMETERS
 *
 * Examples:
 * otpauth://totp/Example:alice@google.com?secret=JBSWY3DPEHPK3PXP&issuer=Example
 * otpauth://totp/ACME%20Co:john.doe@email.com?secret=HXDMVJECJJWSRB3HWIZR4IFUGFTMXBOZ&issuer=ACME%20Co&algorithm=SHA1&digits=6&period=30
 *
 * Parameters:
 *
 *  => secret (REQUIRED)
 *  => issuer (OPTIMAL, unused in this application)
 *  => algorithm (OPTIMAL, default is "SHA1")
 *  => digits (OPTIMAL, default is "6")
 *  => counter (REQUIRED if type is "HOTP", otherwise unused)
 *  => period (OPTIMAL only if type is "TOTP", otherwise unused, default is "30")
 *
 * Note about issuer prefixes:
 *
 *  This application doesn't use "issuers", the prefix is part of the label as-is.
 *
 */

// TODO: currently only parsing is supported
// TODO: write generator

class otpauthURI
{
    class UriComponent;

public:
    otpauthURI();
    otpauthURI(const std::string &uri);
    ~otpauthURI();

    enum Type {
        Invalid = -1,
        TOTP,
        HOTP,
    };

    inline const auto &type() const
    { return _type; }

    inline const auto &label() const
    { return _label; }

    inline const auto &secret() const
    { return _params.at("secret"); }
    inline const auto &algorithm() const
    { return _params.at("algorithm"); }
    inline const auto &digits() const
    { return _params.at("digits"); }

    std::uint8_t digitsNumber() const;

    inline const std::string &counter() const
    {
        static const std::string empty;
        if (_params.count("counter") != 0)
        {
            return _params.at("counter");
        }
        else
        {
            return empty;
        }
    }
    std::uint32_t counterNumber() const;

    inline const std::string &period() const
    {
        static const std::string empty;
        if (_params.count("period") != 0)
        {
            return _params.at("period");
        }
        else
        {
            return empty;
        }
    }
    std::uint32_t periodNumber() const;

    inline const auto &params() const
    { return _params; }

    inline bool empty() const
    { return uri.empty(); }

    inline bool valid() const
    { return _valid; }

private:
    static const std::string OTPAUTH_PREFIX;
    std::string uri;
    bool _valid = false;

    Type _type = Invalid;
    std::string _label;
    std::map<std::string, std::string> _params;

    void setTypeEnum(const std::string &type);

    // helper class to decode and encode URI components (percent signs)
    class UriComponent
    {
    public:
        UriComponent(const std::string &uri, const std::string &prefix = OTPAUTH_PREFIX);
        ~UriComponent();

        const std::string encoded() const;
        const std::string decoded() const;

    private:
        std::string prefix;
        std::string uri;
    };
};

#endif // OTPAUTHURI_HPP
