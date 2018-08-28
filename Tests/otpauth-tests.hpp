#ifndef OTPAUTHTESTS_HPP
#define OTPAUTHTESTS_HPP

#include <bandit/bandit.h>

using namespace snowhouse;
using namespace bandit;

#include <Core/Tools/otpauthURI.hpp>

#include <Core/Tokens/TOTPToken.hpp>
#include <Core/Tokens/HOTPToken.hpp>

go_bandit([]{
    describe("otpauth URI Test", []{
        it("[parse valid 1]", [&]{
            otpauthURI uri("otpauth://totp/Example:alice@google.com?secret=JBSWY3DPEHPK3PXP&issuer=Example");
            AssertThat(uri.valid(), Equals(true));
            AssertThat(uri.label(), Equals(std::string("Example:alice@google.com")));
            AssertThat(uri.type(), Equals(otpauthURI::TOTP));
            AssertThat(uri.secret(), Equals(std::string("JBSWY3DPEHPK3PXP")));
            AssertThat(uri.algorithm(), Equals(std::string("SHA1")));
        });

        it("[parse valid 2]", [&]{
            otpauthURI uri("otpauth://totp/ACME%20Co:john.doe@email.com?secret=HXDMVJECJJWSRB3HWIZR4IFUGFTMXBOZ&issuer=ACME%20Co&algorithm=SHA256&digits=6&period=30");
            AssertThat(uri.valid(), Equals(true));
            AssertThat(uri.label(), Equals(std::string("ACME Co:john.doe@email.com")));
            AssertThat(uri.type(), Equals(otpauthURI::TOTP));
            AssertThat(uri.secret(), Equals(std::string("HXDMVJECJJWSRB3HWIZR4IFUGFTMXBOZ")));
            AssertThat(uri.algorithm(), Equals(std::string("SHA256")));
        });

        it("[parse invalid]", [&]{
            otpauthURI uri("otpauth://totp/Label?");
            AssertThat(uri.valid(), Equals(false));
        });

        it("[write totp]", [&]{
            TOTPToken totp("Label with space");
            totp.setSecret("HXDMVJECJJWSRB3HWIZR4IFUGFTMXBOZ");
            const auto uri = otpauthURI::fromOtpToken(&totp);
            AssertThat(uri.label(), Equals(std::string("Label with space")));
            AssertThat(uri.to_s(), Equals(std::string("otpauth://totp/Label%20with%20space?secret=HXDMVJECJJWSRB3HWIZR4IFUGFTMXBOZ&digits=6&period=30&algorithm=SHA1")));
        });

        it("[write hotp]", [&]{
            HOTPToken hotp("Label with space");
            hotp.setSecret("HXDMVJECJJWSRB3HWIZR4IFUGFTMXBOZ");
            const auto uri = otpauthURI::fromOtpToken(&hotp);
            AssertThat(uri.label(), Equals(std::string("Label with space")));
            AssertThat(uri.to_s(), Equals(std::string("otpauth://hotp/Label%20with%20space?secret=HXDMVJECJJWSRB3HWIZR4IFUGFTMXBOZ&digits=6&period=0&counter=0&algorithm=SHA1")));
        });
    });
});

#endif // OTPAUTHTESTS_HPP
