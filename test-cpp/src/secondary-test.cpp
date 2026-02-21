// This file verifies that the generated reflection header can be included
// from multiple translation units without causing duplicate symbol errors.

#include "catch2/catch2.hpp"

#include "reflection.hpp"
#include "types.hpp"

using namespace FirstNamespace::SecondNamespace;

TEST_CASE("Multi-TU inclusion", "[refl]")
{
    REQUIRE(strcmp(Obs::Enum<Fruit>::GetName(), "Fruit") == 0);
    REQUIRE(strcmp(Obs::Class<DataStruct>::GetName(), "DataStruct") == 0);
}
