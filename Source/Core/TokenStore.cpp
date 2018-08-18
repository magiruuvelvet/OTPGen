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

TokenStore::Status TokenStore::addToken(const Token &token)
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

    // add token if not already in the list
    if (!this->contains(token->label()))
    {
        _tokens.emplace_back(token);
        return Success;
    }

    return AlreadyInStore;
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

    _tokens.remove_if([&](const auto &value) {
        return value->label() == label;
    });
}

bool TokenStore::renameToken(const OTPToken::Label &oldLabel, const OTPToken::Label &newLabel)
{
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
