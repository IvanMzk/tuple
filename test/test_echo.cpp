#include "catch.hpp"
#include "echo.hpp"

TEST_CASE("echo","[echo]"){
    using echo_::echo;
    REQUIRE(echo("")=="");
    REQUIRE(echo("1")=="1");
    REQUIRE(echo("1")!="0");
    REQUIRE(echo("string")!="String");
}