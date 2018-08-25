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

    bool swapTokens(const OTPToken::Label &token1, const OTPToken::Label &token2);
    bool moveToken(const OTPToken::Label &token, std::size_t pos);
    bool moveTokenBelow(const OTPToken::Label &token, const OTPToken::Label &below);

    bool contains(const OTPToken::Label &label) const;
    bool empty() const;
    void clear();

    OTPToken *tokenAt(const OTPToken::Label &label);
    const OTPToken *tokenAt(const OTPToken::Label &label) const;

    TokenList &tokens();
    std::size_t tokenCount() const;

private:
    TokenStore();

    static void moveRange(std::size_t start, std::size_t length, std::size_t dst, TokenList &list);
    void addTokenUnsafe(const Token &token);

    TokenList _tokens;
};

#endif // TOKENSTORE_HPP
