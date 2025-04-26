#include <cstdio>
#include <string>

#include "catch2/catch2.hpp"

#include "reflection.hpp"
#include "types.hpp"

using namespace FirstNamespace::SecondNamespace;

TEST_CASE("Enum to name secondary", "[refl][enum]")
{
    std::string enum_name = Obs::Enum<Fruit>::GetEnumName();
    REQUIRE(enum_name == "Fruit");
    std::string full_enum_name = Obs::Enum<Fruit>::GetFullEnumName();
    REQUIRE(full_enum_name == "FirstNamespace::SecondNamespace::Fruit");

    REQUIRE(Obs::Enum<Fruit>::GetUnderlyingValue(Obs::Enum<Fruit>::k_end) == 8);

    REQUIRE(strcmp(Obs::Enum<Fruit>::GetValueName(Fruit::Apple), "Apple") == 0);
    REQUIRE(strcmp(Obs::Enum<Fruit>::GetValueName(Fruit::Orange), "Orange") == 0);
    REQUIRE(strcmp(Obs::Enum<Fruit>::GetValueName(Fruit::Banana), "Banana") == 0);

    REQUIRE(Obs::Enum<Fruit>::GetValue("Apple") == Fruit::Apple);
    REQUIRE(Obs::Enum<Fruit>::GetValue("Orange") == Fruit::Orange);
    REQUIRE(Obs::Enum<Fruit>::GetValue("Banana") == Fruit::Banana);

    REQUIRE(Obs::Enum<Fruit>::GetValue(5) == Fruit::Apple);
    REQUIRE(Obs::Enum<Fruit>::GetValue(6) == Fruit::Orange);
    REQUIRE(Obs::Enum<Fruit>::GetValue(7) == Fruit::Banana);

    REQUIRE(Obs::Enum<Fruit>::GetUnderlyingValue(Fruit::Apple) == 5);
    REQUIRE(Obs::Enum<Fruit>::GetUnderlyingValue(Fruit::Orange) == 6);
    REQUIRE(Obs::Enum<Fruit>::GetUnderlyingValue(Fruit::Banana) == 7);
}