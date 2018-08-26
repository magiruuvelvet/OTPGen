#ifndef QRCODETEST_HPP
#define QRCODETEST_HPP

#include <bandit/bandit.h>

using namespace snowhouse;
using namespace bandit;

#include <Core/Tools/QRCode.hpp>

go_bandit([]{
    describe("QRCode Test", []{
        it("[valid PNG]", [&]{
            std::string data;
            auto res = QRCode::decode("QRCodes/valid.png", data);
            AssertThat(res, Equals(true));
            AssertThat(data, Equals(std::string("otpauth://totp/Example:alice@google.com?secret=JBSWY3DPEHPK3PXP&issuer=Example")));
        });

        it("[valid JPG]", [&]{
            std::string data;
            auto res = QRCode::decode("QRCodes/valid.jpg", data);
            AssertThat(res, Equals(true));
            AssertThat(data, Equals(std::string("otpauth://totp/Example:alice@google.com?secret=JBSWY3DPEHPK3PXP&issuer=Example")));
        });

        it("[invalid PNG]", [&]{
            std::string data;
            auto res = QRCode::decode("QRCodes/invalid.png", data);
            AssertThat(res, Equals(true));
            AssertThat(data, Equals(std::string("test")));
        });

        it("[file doesn't exists]", [&]{
            std::string data;
            auto res = QRCode::decode("QRCodes/nosuchfile", data);
            AssertThat(res, Equals(false));
            AssertThat(data, Equals(std::string()));
        });
    });
});

#endif // QRCODETEST_HPP
