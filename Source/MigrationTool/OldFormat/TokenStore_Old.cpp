#include "TokenStore_Old.hpp"

#include <algorithm>

TokenStore_Old::TokenStore_Old()
{
}

TokenStore_Old *TokenStore_Old::i()
{
    static std::shared_ptr<TokenStore_Old> instance(new TokenStore_Old());
    return instance.get();
}

TokenStore_Old::~TokenStore_Old()
{
    _tokens.clear();
}

TokenStore_Old::Status TokenStore_Old::addToken(const Token &token, bool override)
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
                    case OTPToken_Old::TOTP: _token = std::make_shared<TOTPToken_Old>(
                        token->_label, token->_icon, token->_secret, token->_digits, token->_period, token->_counter, token->_algorithm);
                        break;

                    case OTPToken_Old::HOTP: _token = std::make_shared<HOTPToken_Old>(
                        token->_label, token->_icon, token->_secret, token->_digits, token->_period, token->_counter, token->_algorithm);
                        break;

                    case OTPToken_Old::Steam: _token = std::make_shared<SteamToken_Old>(
                        token->_label, token->_icon, token->_secret, token->_digits, token->_period, token->_counter, token->_algorithm);
                        break;

                    case OTPToken_Old::Authy: _token = std::make_shared<AuthyToken_Old>(
                        token->_label, token->_icon, token->_secret, token->_digits, token->_period, token->_counter, token->_algorithm);
                        break;

                   case OTPToken_Old::None:
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

TokenStore_Old::Status TokenStore_Old::addToken(const OTPToken_Old *token, bool override)
{
    return addToken(token->clone(), override);
}

void TokenStore_Old::addTokenUnsafe(const Token &token)
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

void TokenStore_Old::removeToken(const OTPToken_Old::Label &label)
{
    if (label.empty())
        return;

    _tokens.erase(std::remove_if(_tokens.begin(), _tokens.end(), [&](const auto &value){
        return value->label() == label;
    }), _tokens.end());
}

bool TokenStore_Old::renameToken(const OTPToken_Old::Label &oldLabel, const OTPToken_Old::Label &newLabel)
{
    if (this->contains(newLabel))
    {
        return false;
    }

    for (auto&& token : _tokens)
    {
        if (token->label() == oldLabel)
        {
            token->setLabel(newLabel);
            return true;
        }
    }

    return false;
}

bool TokenStore_Old::swapTokens(const OTPToken_Old::Label &token1, const OTPToken_Old::Label &token2)
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

bool TokenStore_Old::moveToken(const OTPToken_Old::Label &token, std::size_t pos)
{
    if (pos >= _tokens.size())
    {
        return false;
    }

    auto oldPos = std::find_if(_tokens.begin(), _tokens.end(), [&](const auto &value){
        return value->label() == token;
    });
    if (oldPos == _tokens.end())
    {
        return false;
    }

    moveRange(std::distance(_tokens.begin(), oldPos), 1, pos, _tokens);
    return true;
}

bool TokenStore_Old::moveTokenBelow(const OTPToken_Old::Label &token, const OTPToken_Old::Label &below)
{
    auto pos1 = std::find_if(_tokens.begin(), _tokens.end(), [&](const auto &value){
        return value->label() == token;
    });
    if (pos1 == _tokens.end())
    {
        return false;
    }

    auto pos2 = std::find_if(_tokens.begin(), _tokens.end(), [&](const auto &value){
        return value->label() == below;
    });
    if (pos2 == _tokens.end())
    {
        return false;
    }

    moveRange(std::distance(_tokens.begin(), pos1), 1, std::distance(_tokens.begin(), pos2) + 1, _tokens);
    return true;
}

void TokenStore_Old::moveRange(std::size_t start, std::size_t length, std::size_t dst, TokenList &list)
{
    const std::size_t final_dst = dst > start ? dst - length : dst;

    TokenList tmp(list.begin() + start, list.begin() + start + length);
    list.erase(list.begin() + start, list.begin() + start + length);
    list.insert(list.begin() + final_dst, tmp.begin(), tmp.end());
}

bool TokenStore_Old::contains(const OTPToken_Old::Label &label) const
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

bool TokenStore_Old::empty() const
{
    return _tokens.empty();
}

void TokenStore_Old::clear()
{
    _tokens.clear();
}

OTPToken_Old *TokenStore_Old::tokenAt(const OTPToken_Old::Label &label)
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

const OTPToken_Old *TokenStore_Old::tokenAt(const OTPToken_Old::Label &label) const
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

TokenStore_Old::TokenList &TokenStore_Old::tokens()
{
    return _tokens;
}

std::size_t TokenStore_Old::tokenCount() const
{
    return _tokens.size();
}
