#include "OTPToken.hpp"

#include <algorithm>
#include <chrono>

// limits of otp gen
const int OTPToken::min_digits = 3;
const int OTPToken::max_digits = 10;
const int OTPToken::min_period = 1;
const int OTPToken::max_period = 120;
const int OTPToken::min_counter = 0;
const int OTPToken::max_counter = 0x7FFFFFFF;

OTPToken::OTPToken()
{
    _type = None;
    _typeName.clear();
}

OTPToken::OTPToken(const OTPToken *other)
{
    _type = other->_type;
    _typeName = other->_typeName;

    _label = other->_label;
    _icon = other->_icon;

    _secret = other->_secret;
    _digits = other->_digits;
    _period = other->_period;
    _counter = other->_counter;
    _algorithm = other->_algorithm;
}

OTPToken::OTPToken(const Label &label)
{
    _label = label;
}

OTPToken::~OTPToken()
{
    _type = None;
    _typeName.clear();

    _label.clear();
    _icon.clear();

    _secret.clear();
    _digits = 0U;
    _period = 0U;
    _counter = 0U;
    _algorithm = Invalid;
}


void OTPToken::setAlgorithm(const std::string &_algo)
{
    std::string algo = _algo;
    std::transform(algo.begin(), algo.end(), algo.begin(), [](unsigned char c) {
        return std::toupper(c);
    });

    ShaAlgorithm tmp;

    if (algo == "SHA1")
    {
        tmp = SHA1;
    }
    else if (algo == "SHA256")
    {
        tmp = SHA256;
    }
    else if (algo == "SHA512")
    {
        tmp = SHA512;
    }
    else
    {
        tmp = Invalid;
    }

    this->setAlgorithm(tmp);
}

const std::string OTPToken::algorithmString() const
{
    switch (_algorithm)
    {
        case SHA1:    return "SHA1";
        case SHA256:  return "SHA256";
        case SHA512:  return "SHA512";
    }

    return "(invalid)";
}

#ifdef OTPGEN_DEBUG
#include <sstream>
const std::string OTPToken::debug() const
{
    std::stringstream msg;
    msg << "OTPToken {\n";
    msg << "  type      = " << _typeName << "\n";
    msg << "  label     = " << _label << "\n";
    msg << "  secret    = " << (_secret.empty() ? "(empty)" : "(not empty)") << "\n";
    msg << "  digits    = " << std::to_string(_digits) << "\n";
    msg << "  period    = " << std::to_string(_period) << "\n";
    msg << "  counter   = " << std::to_string(_counter) << "\n";
    msg << "  algorithm = " << algorithmString() << "\n";
    msg << "}";
    return msg.str();
}
#endif

bool OTPToken::valid() const
{
    OTPGenErrorCode error = OTPGenErrorCode::Valid;
    const auto &token = this->generateToken(&error);
    return !(token.empty() || error != OTPGenErrorCode::Valid);
}

std::uint32_t OTPToken::remainingTokenValidity() const
{
    if (_period == 0U)
    {
        return 0U;
    }

    // get remaining seconds since last minute
    auto now = std::time(nullptr);
    auto local = std::localtime(&now);

    // calculate token validity with 1 second update threshold
    auto sec_expired = local->tm_sec;
    auto token_validity = ( static_cast<int>(_period) - sec_expired );
    if (token_validity < 0)
    {
        token_validity = ( static_cast<int>(_period) - (sec_expired % static_cast<int>(_period)) ) + 1;
    }
    else
    {
        token_validity++;
    }

    return static_cast<std::uint32_t>(token_validity);
}

bool OTPToken::check_empty(const TokenString &secret, OTPGenErrorCode *error)
{
    if (secret.empty())
    {
        if (error)
        {
            (*error) = OTPGenErrorCode::InvalidBase32Input;
        }
        return true;
    }
    return false;
}
