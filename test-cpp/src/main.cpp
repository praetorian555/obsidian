#include <cstdio>
#include <string>

#include "catch2/catch2.hpp"

#include "reflection.hpp"
#include "types.hpp"

using namespace FirstNamespace::SecondNamespace;

int main(int argc, char* argv[])
{
    Catch::Session session;
    const int return_code = session.applyCommandLine(argc, argv);
    if (return_code != 0)
    {
        return return_code;
    }
    return session.run();
}

TEST_CASE("Enum to name", "[refl][enum]")
{
    std::string enum_name = Obs::Enum<Fruit>::GetEnumName();
    REQUIRE(enum_name == "Fruit");
    std::string full_enum_name = Obs::Enum<Fruit>::GetFullEnumName();
    REQUIRE(full_enum_name == "FirstNamespace::SecondNamespace::Fruit");

    REQUIRE(Obs::Enum<Fruit>::GetName(Fruit::Apple) == "Apple");
    REQUIRE(Obs::Enum<Fruit>::GetName(Fruit::Orange) == "Orange");
    REQUIRE(Obs::Enum<Fruit>::GetName(Fruit::Banana) == "Banana");
}