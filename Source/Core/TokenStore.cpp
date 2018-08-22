#include "TokenStore.hpp"

#include <algorithm>

TokenStore::TokenStore()
{
}

TokenStore *TokenStore::i()
{
    static std::shared_ptr<TokenStore> instance(new TokenStore());
    return instance.get();
}

TokenStore::~TokenStore()
{
    _tokens.clear();
}

TokenStore::Status TokenStore::addToken(const Token &token, bool override)
{
    // do nothing on nullptr
    if (!token)
        return Nullptr;

    // label should not be empty
    if (token->label().empty())
        return LabelEmpty;

    // secret must not be empty
    if (token->secret().empty())
        return SecretEmpty;

    if (override)
    {
        for (auto&& _token : _tokens)
        {
            if (_token->label() == token->label())
            {
                switch (token->type())
                {
                    case OTPToken::TOTP: _token = std::make_shared<TOTPToken>(TOTPToken(
                        token->_label, token->_icon, token->_secret, token->_digits, token->_period, token->_counter, token->_algorithm));
                        break;

                    case OTPToken::HOTP: _token = std::make_shared<HOTPToken>(HOTPToken(
                        token->_label, token->_icon, token->_secret, token->_digits, token->_period, token->_counter, token->_algorithm));
                        break;

                    case OTPToken::Steam: _token = std::make_shared<SteamToken>(SteamToken(
                        token->_label, token->_icon, token->_secret, token->_digits, token->_period, token->_counter, token->_algorithm));
                        break;

                    case OTPToken::Authy: _token = std::make_shared<AuthyToken>(AuthyToken(
                        token->_label, token->_icon, token->_secret, token->_digits, token->_period, token->_counter, token->_algorithm));
                        break;

                   case OTPToken::None:
                        return NotOverridden;
                        break;
                }

                return Success;
            }
        }
        return NotOverridden;
    }
    else
    {
        // add token if not already in the list
        if (!this->contains(token->label()))
        {
            _tokens.emplace_back(token);
            return Success;
        }

        return AlreadyInStore;
    }
}

void TokenStore::addTokenUnsafe(const Token &token)
{
    // do nothing on nullptr
    if (!token)
        return;

    // label should not be empty
    if (token->label().empty())
        return;

    // add token if not already in the list
    if (!this->contains(token->label()))
    {
        _tokens.emplace_back(token);
    }
}

void TokenStore::removeToken(const OTPToken::Label &label)
{
    if (label.empty())
        return;

    _tokens.erase(std::remove_if(_tokens.begin(), _tokens.end(), [&](const auto &value){
        return value->label() == label;
    }), _tokens.end());
}

bool TokenStore::renameToken(const OTPToken::Label &oldLabel, const OTPToken::Label &newLabel)
{
    if (this->contains(newLabel))
    {
        return false;
    }

    for (auto&& token : _tokens)
    {
        if (token->label() == oldLabel)
        {
            token->label() = newLabel;
            return true;
        }
    }

    return false;
}

bool TokenStore::swapTokens(const OTPToken::Label &token1, const OTPToken::Label &token2)
{
    auto pos1 = std::find_if(_tokens.begin(), _tokens.end(), [&](const auto &value){
        return value->label() == token1;
    });
    if (pos1 == _tokens.end())
    {
        return false;
    }

    auto pos2 = std::find_if(_tokens.begin(), _tokens.end(), [&](const auto &value){
        return value->label() == token2;
    });
    if (pos2 == _tokens.end())
    {
        return false;
    }

    std::iter_swap(pos1, pos2);
    return true;
}

bool TokenStore::contains(const OTPToken::Label &label) const
{
    for (auto&& token : _tokens)
    {
        if (token->label() == label)
        {
            return true;
        }
    }
    return false;
}

bool TokenStore::empty() const
{
    return _tokens.empty();
}

void TokenStore::clear()
{
    _tokens.clear();
}

OTPToken *TokenStore::tokenAt(const OTPToken::Label &label)
{
    for (auto&& token : _tokens)
    {
        if (token->label() == label)
        {
            return token.get();
        }
    }
    return nullptr;
}

const OTPToken *TokenStore::tokenAt(const OTPToken::Label &label) const
{
    for (auto&& token : _tokens)
    {
        if (token->label() == label)
        {
            return token.get();
        }
    }
    return nullptr;
}

TokenStore::TokenList &TokenStore::tokens()
{
    return _tokens;
}

std::size_t TokenStore::tokenCount() const
{
    return _tokens.size();
}
