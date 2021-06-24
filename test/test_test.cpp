#include "catch.hpp"
#include "./catch2/trompeloeil.hpp"
#include "test.hpp"

TEST_CASE("echo","[test]"){
    using test_::echo;
    REQUIRE(echo("1")=="1");
    REQUIRE(echo("1")!="0");
}