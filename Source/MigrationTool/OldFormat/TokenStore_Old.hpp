#ifndef TOKENSTORE_OLD_HPP
#define TOKENSTORE_OLD_HPP

#include "Tokens_Old.hpp"

#include <memory>
#include <vector>

class TokenStore_Old
{
    friend class TokenDatabase_Old;

public:
    static TokenStore_Old *i();
    ~TokenStore_Old();

    using Token = std::shared_ptr<OTPToken_Old>;
    using TokenList = std::vector<std::shared_ptr<OTPToken_Old>>;

    enum Status {
        Success = 0,
        Nullptr,
        LabelEmpty,
        SecretEmpty,
        AlreadyInStore,
        NotOverridden,
    };

    Status addToken(const Token &token, bool override = false);
    Status addToken(const OTPToken_Old *token, bool override = false);
    void removeToken(const OTPToken_Old::Label &label);
    bool renameToken(const OTPToken_Old::Label &oldLabel, const OTPToken_Old::Label &newLabel);

    bool swapTokens(const OTPToken_Old::Label &token1, const OTPToken_Old::Label &token2);
    bool moveToken(const OTPToken_Old::Label &token, std::size_t pos);
    bool moveTokenBelow(const OTPToken_Old::Label &token, const OTPToken_Old::Label &below);

    bool contains(const OTPToken_Old::Label &label) const;
    bool empty() const;
    void clear();

    OTPToken_Old *tokenAt(const OTPToken_Old::Label &label);
    const OTPToken_Old *tokenAt(const OTPToken_Old::Label &label) const;

    TokenList &tokens();
    std::size_t tokenCount() const;

private:
    TokenStore_Old();

    static void moveRange(std::size_t start, std::size_t length, std::size_t dst, TokenList &list);
    void addTokenUnsafe(const Token &token);

    TokenList _tokens;
};

#endif // TOKENSTORE_OLD_HPP
