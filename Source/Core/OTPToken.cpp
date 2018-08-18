#include "OTPToken.hpp"

#include <algorithm>
#include <gcrypt.h>

OTPToken::OTPToken()
{
    _type = None;
    _name.clear();
}

OTPToken::OTPToken(const Label &label)
{
    _label = label;
}

OTPToken::~OTPToken()
{
    _type = None;
    _name.clear();

    _label.clear();

    _secret.clear();
    _digits = 0U;
    _period = 0U;
    _counter = 0U;
    _algorithm = Invalid;
}

const OTPToken::TokenType &OTPToken::type() const
{
    return _type;
}

const std::string &OTPToken::name() const
{
    return _name;
}

OTPToken::Label &OTPToken::label()
{
    return _label;
}

OTPToken::SecretType &OTPToken::secret()
{
    return _secret;
}

OTPToken::DigitType &OTPToken::digits()
{
    return _digits;
}

OTPToken::PeriodType &OTPToken::period()
{
    return _period;
}

OTPToken::CounterType &OTPToken::counter()
{
    return _counter;
}

OTPToken::ShaAlgorithm &OTPToken::algorithm()
{
    return _algorithm;
}

#ifdef OTPGEN_DEBUG
#include <sstream>
const std::string OTPToken::debug() const
{
    std::stringstream msg;
    msg << "OTPToken {\n";
    msg << "  type      = " << _name << "\n";
    msg << "  label     = " << _label << "\n";
    msg << "  secret    = " << (_secret.empty() ? "(empty)" : "(not empty)") << "\n";
    msg << "  digits    = " << std::to_string(_digits) << "\n";
    msg << "  period    = " << std::to_string(_period) << "\n";
    msg << "  counter   = " << std::to_string(_counter) << "\n";
    msg << "  algorithm = " << sha_enum_to_str() << "\n";
    msg << "}";
    return msg.str();
}
#endif

bool OTPToken::valid() const
{
    Error error = VALID;
    const auto &token = this->generateToken(&error);
    return !(token.empty() || error != VALID);
}

int OTPToken::sha_enum_to_gcrypt() const
{
    switch (_algorithm)
    {
        case SHA1:    return GCRY_MD_SHA1;
        case SHA256:  return GCRY_MD_SHA256;
        case SHA512:  return GCRY_MD_SHA512;

        case Invalid: return GCRY_MD_NONE;
    }

    return GCRY_MD_NONE;
}

const std::string OTPToken::sha_enum_to_str() const
{
    switch (_algorithm)
    {
        case SHA1:    return "SHA1";
        case SHA256:  return "SHA256";
        case SHA512:  return "SHA512";

        case Invalid: return "(invalid)";
    }

    return "(invalid)";
}

bool OTPToken::check_empty(const TokenString &secret, Error *error)
{
    if (secret.empty())
    {
        if (error)
        {
            (*error) = INVALID_B32_INPUT;
        }
        return true;
    }
    return false;
}
