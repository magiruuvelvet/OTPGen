#ifndef AUTHY_HPP
#define AUTHY_HPP

#include <Core/Tokens/TOTPToken.hpp>
#include <Core/Tokens/AuthyToken.hpp>

#include <vector>

namespace Import {

class Authy
{
    Authy() = delete;

public:
    enum Format {
        XML,
        JSON,
    };

    static bool importTOTP(const std::string &file, std::vector<TOTPToken> &target, const Format &format);
    static bool importNative(const std::string &file, std::vector<AuthyToken> &target, const Format &format);

private:
    enum AuthyXMLType {
        TOTP,
        Native,
    };

    static const std::string hexToBase32Rfc4648(const std::string &hex);

    static bool prepare(const std::string &file, const Format &format, const AuthyXMLType &type, std::string &json);
    static bool extractJSON(const std::string &xml, const AuthyXMLType &type, std::string &json);
};

}

#endif // AUTHY_HPP
