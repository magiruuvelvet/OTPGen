#include "otpauthURI.hpp"

#include <Core/Tokens/OTPToken.hpp>

#include <cstring>
#include <vector>
#include <algorithm>
#include <regex>

const std::string otpauthURI::OTPAUTH_PREFIX = "otpauth://";

otpauthURI::otpauthURI()
{
}

otpauthURI::otpauthURI(const std::string &uri)
{
    if (uri.size() >= OTPAUTH_PREFIX.size() && uri.substr(0, OTPAUTH_PREFIX.size()) == OTPAUTH_PREFIX)
    {
        this->uri = uri.substr(OTPAUTH_PREFIX.size());
    }

    if (!empty())
    {
        // extract type
        auto delim = this->uri.find_first_of('/');
        if (delim == std::string::npos)
        {
            setTypeEnum(this->uri);
            return;
        }
        else
        {
            setTypeEnum(this->uri.substr(0, delim));
            if (_type == Invalid)
            {
                return;
            }
        }

        ++delim;

        // extract label, use URI decoding
        auto delim2 = this->uri.find_first_of('?', delim);
        if (delim2 == std::string::npos)
        {
            UriComponent label(this->uri.substr(delim));
            _label = label.decoded();
            return;
        }
        else
        {
            UriComponent label(this->uri.substr(delim, delim2 - delim));
            _label = label.decoded();
        }

        ++delim2;

        // extract parameters
        auto params_str = this->uri.substr(delim2);
        if (params_str.empty() || params_str.find('=') == std::string::npos)
        {
            return;
        }

        std::regex re(R"(\&)");
        std::sregex_token_iterator first{
            params_str.begin(), params_str.end(), re, -1
        }, last;
        std::vector<std::string> params_vec = {first, last};

        for (auto&& p : params_vec)
        {
            auto delim = p.find('=');
            if (delim == std::string::npos)
            {
                continue;
            }

            auto key = p.substr(0, delim);
            auto value = p.substr(delim + 1);

            // URI decode issuer
            if (key == "issuer")
            {
                UriComponent issuer(value);
                value = issuer.decoded();
            }

            _params.insert({key, value});
        }

        // parameters may not be empty
        if (_params.empty())
        {
            return;
        }

        // check for mandatory fields
        if (_params.count("secret") != 1)
        {
            _params.clear();
            return;
        }
        if (_type == HOTP && _params.count("counter") != 1)
        {
            _params.clear();
            return;
        }

        // add defaults when missing
        if (_params.count("algorithm") == 0)
        {
            _params.insert({"algorithm", "SHA1"});
        }
        if (_params.count("digits") == 0)
        {
            _params.insert({"digits", "6"});
        }

        if (_type == TOTP && _params.count("period") == 0)
        {
            _params.insert({"period", "30"});
        }

        // set valid if reached here
        _valid = true;
    }
}

otpauthURI::~otpauthURI()
{
    uri.clear();
    _label.clear();
    _params.clear();
}

otpauthURI otpauthURI::fromOtpToken(const OTPToken *token)
{
    if (!token)
    {
        return otpauthURI();
    }

    auto t = const_cast<OTPToken*>(token);

    std::string uri = OTPAUTH_PREFIX;

    switch (t->type())
    {
        case OTPToken::TOTP:  uri.append("totp/"); break;
        case OTPToken::HOTP:  uri.append("hotp/"); break;
        case OTPToken::Authy: uri.append("totp/"); break;
        case OTPToken::Steam: uri.append("totp/"); break;
        default: return otpauthURI(); break;
    }

    UriComponent label(t->label());
    uri.append(label.encoded());

    uri.append("?");
    uri.append("secret=" + t->secret());

    if (t->type() != OTPToken::Steam)
    {
        uri.append("&");
        uri.append("digits=" + std::to_string(t->digits()));

        uri.append("&");
        uri.append("period=" + std::to_string(t->period()));

        if (t->type() == OTPToken::HOTP)
        {
            uri.append("&");
            uri.append("counter=" + std::to_string(t->counter()));
        }

        uri.append("&");
        uri.append("algorithm=" + t->algorithmString());
    }

    return otpauthURI(uri);
}

std::uint8_t otpauthURI::digitsNumber() const
{
    return static_cast<std::uint8_t>(std::stoul(digits()));
}

std::uint32_t otpauthURI::counterNumber() const
{
    return static_cast<std::uint32_t>(std::stoul(counter()));
}

std::uint32_t otpauthURI::periodNumber() const
{
    return static_cast<std::uint32_t>(std::stoul(period()));
}

void otpauthURI::setTypeEnum(const std::string &type)
{
    if (type == "totp")
    {
        _type = TOTP;
    }
    else if (type == "hotp")
    {
        _type = HOTP;
    }
    else
    {
        _type = Invalid;
    }
}

otpauthURI::UriComponent::UriComponent(const std::string &uri, const std::string &prefix)
    : prefix(prefix), uri(uri)
{
    if (uri.size() >= prefix.size() && uri.substr(0, prefix.size()) == prefix)
    {
        this->uri = uri.substr(prefix.size());
    }
    else
    {
        this->prefix.clear();
    }
}

otpauthURI::UriComponent::~UriComponent()
{
    prefix.clear();
    uri.clear();
}

const std::string otpauthURI::UriComponent::encoded() const
{
    std::string new_str = "";
    char c;
    int ic;
    const char *chars = uri.c_str();
    char bufHex[10];
    auto len = strlen(chars);

    for (auto i = 0U; i < len; i++)
    {
        c = chars[i];
        ic = c;
//        if (c == ' ')
//        {
//            new_str += '+';
//        }
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
        {
            new_str += c;
        }
        else
        {
            std::sprintf(bufHex, "%X", c);
            if (ic < 16)
            {
                new_str += "%0";
            }
            else
            {
                new_str += "%";
            }
            new_str += bufHex;
        }
    }
    return prefix + new_str;
}

const std::string otpauthURI::UriComponent::decoded() const
{
    std::string ret;
    char ch;
    unsigned int i, ii;
    auto len = uri.length();

    for (i = 0U; i < len; i++)
    {
        if (uri[i] != '%')
        {
            if (uri[i] == '+')
            {
                ret += ' ';
            }
            else
            {
                ret += uri[i];
            }
        }
        else
        {
            std::sscanf(uri.substr(i + 1, 2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            ret += ch;
            i = i + 2;
        }
    }
    return prefix + ret;
}
