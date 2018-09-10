#ifndef OTPGENTESTS_HPP
#define OTPGENTESTS_HPP

#include <bandit/bandit.h>

using namespace snowhouse;
using namespace bandit;

#include <OTPGen.hpp>

// NOTICE:
//   code was tested with real token secrets for TOTP and Steam
//   and all of them worked, login was successful

go_bandit([]{
    describe("OTPGen Test", []{
        it("[computeTOTP 1]", [&]{
            // test totp token at a fixed time, result must be always the same
            const auto res = OTPGen::computeTOTP(1536573862, "XYZA123456KDDK83D", 6, 30, OTPToken::SHA1);
            AssertThat(res, Equals(std::string("122810")));
        });

        it("[computeTOTP 2]", [&]{
            // test totp token at a fixed time, result must be always the same
            const auto res = OTPGen::computeTOTP(1536573862, "XYZA123456KDDK83D28273", 7, 10, OTPToken::SHA1);
            AssertThat(res, Equals(std::string("8578249")));
        });

        it("[computeHOTP]", [&]{
            // test hotp token with a fixed counter at 12
            const auto res = OTPGen::computeHOTP("XYZA123456KDDK83D", 12, 6, OTPToken::SHA1);
            AssertThat(res, Equals(std::string("534003")));
        });

        it("[computeSteam]", [&]{
            // test steam token at a fixed time, result must be always the same
            const auto res = OTPGen::computeSteam(1536573862, "ABC30WAY33X57CCBU3EAXGDDMX35S39M");
            AssertThat(res, Equals(std::string("GQTTM")));
        });
    });
});

#endif // OTPGENTESTS_HPP
