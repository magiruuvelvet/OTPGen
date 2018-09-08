#ifndef STEAMBASETEST_HPP
#define STEAMBASETEST_HPP

#include <bandit/bandit.h>

using namespace snowhouse;
using namespace bandit;

#include <Tokens/SteamToken.hpp>

// NOTICE: this was also tested with an actual Steam token and it works

go_bandit([]{
    describe("Steam base test", []{
        it("[base-64 import]", [&]{
            const auto res = SteamToken::convertBase64Secret("dGhpcyBpcyBhIHRlc3Q="); // value: "this is a test"
            AssertThat(res, Equals(std::string("ORUGS4ZANFZSAYJAORSXG5A"))); // RFC 4648 base-32 result
        });
    });
});

#endif // STEAMBASETEST_HPP
