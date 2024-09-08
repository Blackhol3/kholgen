#include <catch2/catch_test_macros.hpp>

#include "Subject.h"
#include "Teacher.h"

TEST_CASE("getTotalVolume") {
    Subject subject("subject", "subject", "s", 1);
    Teacher teacher1("1", "1", subject, {}, 1, 1);
    Teacher teacher2("1", "1", subject, {}, 1, 1.5);
    Teacher teacher3("1", "1", subject, {}, 1, 1.333);
    int const nbWeeks = 3;

    auto const &result1 = teacher1.getTotalVolume(nbWeeks);
    REQUIRE(result1.isExact);
    REQUIRE(result1.value == 3);

    auto const &result2 = teacher2.getTotalVolume(nbWeeks);
    REQUIRE_FALSE(result2.isExact);
    REQUIRE(result2.value == 4);

    auto const &result3 = teacher3.getTotalVolume(nbWeeks);
    REQUIRE(result3.isExact);
    REQUIRE(result3.value == 4);
}
