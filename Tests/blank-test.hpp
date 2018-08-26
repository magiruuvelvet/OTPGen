#ifndef BLANKTEST_HPP
#define BLANKTEST_HPP

#include <bandit/bandit.h>

using namespace snowhouse;
using namespace bandit;

go_bandit([]{
    describe("blank test", []{
        it("[test]", [&]{
            AssertThat(true, Equals(true));
        });
    });
});

#endif // BLANKTEST_HPP
