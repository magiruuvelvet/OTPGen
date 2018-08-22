#ifndef TOKENSTORE_HPP
#define TOKENSTORE_HPP

#include "Tokens.hpp"

#include <memory>
#include <vector>

class TokenStore final
{
    friend class TokenDatabase;

public:
    static TokenStore *i();
    ~TokenStore();

    using Token = std::shared_ptr<OTPToken>;
    using TokenList = std::vector<std::shared_ptr<OTPToken>>;

    enum Status {
        Success = 0,
        Nullptr,
        LabelEmpty,
        SecretEmpty,
        AlreadyInStore,
        NotOverridden,
    };

    Status addToken(const Token &token, bool override = false);
    void removeToken(const OTPToken::Label &label);
    bool renameToken(const OTPToken::Label &oldLabel, const OTPToken::Label &newLabel);

    bool contains(const OTPToken::Label &label) const;
    bool empty() const;
    void clear();

    OTPToken *tokenAt(const OTPToken::Label &label);
    const OTPToken *tokenAt(const OTPToken::Label &label) const;

    TokenList &tokens();
    std::size_t tokenCount() const;

private:
    TokenStore();

    void addTokenUnsafe(const Token &token);

    TokenList _tokens;
};

#endif // TOKENSTORE_HPP
