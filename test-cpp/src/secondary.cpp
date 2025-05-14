#include <cstdio>
#include <string>

#include "catch2/catch2.hpp"

#include "reflection.hpp"
#include "types.hpp"

using namespace FirstNamespace::SecondNamespace;

TEST_CASE("Enum to name secondary", "[refl][enum]")
{
    SECTION("Fruit")
    {
        std::string enum_name = Obs::Enum<Fruit>::GetEnumName();
        REQUIRE(enum_name == "Fruit");
        std::string full_enum_name = Obs::Enum<Fruit>::GetFullEnumName();
        REQUIRE(full_enum_name == "FirstNamespace::SecondNamespace::Fruit");
        std::string description = Obs::Enum<Fruit>::GetDescription();
        REQUIRE(description == "This is a fruit enum.");

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
    SECTION("Vegetable")
    {
        std::string enum_name = Obs::Enum<FirstNamespace::Vegetable>::GetEnumName();
        REQUIRE(enum_name == "Vegetable");
        std::string full_enum_name = Obs::Enum<FirstNamespace::Vegetable>::GetFullEnumName();
        REQUIRE(full_enum_name == "FirstNamespace::Vegetable");
        std::string description = Obs::Enum<FirstNamespace::Vegetable>::GetDescription();
        REQUIRE(description == "This is a vegetable enum.");

        REQUIRE(Obs::Enum<FirstNamespace::Vegetable>::GetUnderlyingValue(Obs::Enum<FirstNamespace::Vegetable>::k_end) == -7);

        REQUIRE(strcmp(Obs::Enum<FirstNamespace::Vegetable>::GetValueName(FirstNamespace::Vegetable::Carrot), "Carrot") == 0);
        REQUIRE(strcmp(Obs::Enum<FirstNamespace::Vegetable>::GetValueName(FirstNamespace::Vegetable::Potato), "Potato") == 0);
        REQUIRE(strcmp(Obs::Enum<FirstNamespace::Vegetable>::GetValueName(FirstNamespace::Vegetable::Cucumber), "Cucumber") == 0);

        REQUIRE(Obs::Enum<FirstNamespace::Vegetable>::GetValue("Carrot") == FirstNamespace::Vegetable::Carrot);
        REQUIRE(Obs::Enum<FirstNamespace::Vegetable>::GetValue("Potato") == FirstNamespace::Vegetable::Potato);
        REQUIRE(Obs::Enum<FirstNamespace::Vegetable>::GetValue("Cucumber") == FirstNamespace::Vegetable::Cucumber);

        REQUIRE(Obs::Enum<FirstNamespace::Vegetable>::GetValue(-10) == FirstNamespace::Vegetable::Carrot);
        REQUIRE(Obs::Enum<FirstNamespace::Vegetable>::GetValue(-9) == FirstNamespace::Vegetable::Potato);
        REQUIRE(Obs::Enum<FirstNamespace::Vegetable>::GetValue(-8) == FirstNamespace::Vegetable::Cucumber);

        REQUIRE(Obs::Enum<FirstNamespace::Vegetable>::GetUnderlyingValue(FirstNamespace::Vegetable::Carrot) == -10);
        REQUIRE(Obs::Enum<FirstNamespace::Vegetable>::GetUnderlyingValue(FirstNamespace::Vegetable::Potato) == -9);
        REQUIRE(Obs::Enum<FirstNamespace::Vegetable>::GetUnderlyingValue(FirstNamespace::Vegetable::Cucumber) == -8);
    }
}

TEST_CASE("Enum collection secondary", "[refl][enum]")
{
        SECTION("Collection info")
    {
        const Obs::EnumEntry* entry = nullptr;
        REQUIRE(Obs::EnumCollection::GetEnum("Vegetable", entry));
        REQUIRE(strcmp(entry->name, "Vegetable") == 0);
        REQUIRE(strcmp(entry->full_name, "FirstNamespace::Vegetable") == 0);
        REQUIRE(strcmp(entry->description, "This is a vegetable enum.") == 0);
        REQUIRE(entry->underlying_type_size == 1);
        REQUIRE(entry->items.size() == 3);
        REQUIRE(Obs::EnumCollection::GetEnum("Fruit", entry));
        REQUIRE(strcmp(entry->name, "Fruit") == 0);
        REQUIRE(strcmp(entry->full_name, "FirstNamespace::SecondNamespace::Fruit") == 0);
        REQUIRE(strcmp(entry->description, "This is a fruit enum.") == 0);
        REQUIRE(entry->underlying_type_size == sizeof(int));
        REQUIRE(entry->items.size() == 3);
        REQUIRE(Obs::EnumCollection::GetEnum("DataType", entry));
        REQUIRE(strcmp(entry->name, "DataType") == 0);
        REQUIRE(strcmp(entry->full_name, "FirstNamespace::SecondNamespace::DataStruct::DataType") == 0);
        REQUIRE(strcmp(entry->description, "") == 0);
        REQUIRE(entry->underlying_type_size == 2);
        REQUIRE(entry->items.size() == 3);
    }
    SECTION("Collection items")
    {
        const Obs::EnumEntry* entry = nullptr;
        REQUIRE(Obs::EnumCollection::GetEnum("Vegetable", entry));
        REQUIRE(strcmp(entry->items[0].name, "Carrot") == 0);
        REQUIRE(static_cast<int8_t>(entry->items[0].value) == -10);
        REQUIRE(strcmp(entry->items[1].name, "Potato") == 0);
        REQUIRE(static_cast<int8_t>(entry->items[1].value) == -9);
        REQUIRE(strcmp(entry->items[2].name, "Cucumber") == 0);
        REQUIRE(static_cast<int8_t>(entry->items[2].value) == -8);

        REQUIRE(Obs::EnumCollection::GetEnum("Fruit", entry));
        REQUIRE(strcmp(entry->items[0].name, "Apple") == 0);
        REQUIRE(strcmp(entry->items[0].description, "") == 0);
        REQUIRE(static_cast<int>(entry->items[0].value) == 5);
        REQUIRE(strcmp(entry->items[1].name, "Orange") == 0);
        REQUIRE(strcmp(entry->items[1].description, "This is orange.") == 0);
        REQUIRE(static_cast<int>(entry->items[1].value) == 6);
        REQUIRE(strcmp(entry->items[2].name, "Banana") == 0);
        REQUIRE(static_cast<int>(entry->items[2].value) == 7);
    }
    SECTION("Collection get item value by name")
    {
        FirstNamespace::Vegetable val = FirstNamespace::Vegetable::Carrot;
        REQUIRE(Obs::EnumCollection::GetValue(&val, "Vegetable", "Potato"));
        REQUIRE(val == FirstNamespace::Vegetable::Potato);

        Fruit val2 = Fruit::Apple;
        REQUIRE(Obs::EnumCollection::GetValue(&val2, "Fruit", "Orange"));
        REQUIRE(val2 == Fruit::Orange);
    }
    SECTION("Collection get enum entry by name")
    {
        const Obs::EnumEntry* entry = nullptr;
        REQUIRE(Obs::EnumCollection::GetEnum("Fruit", entry));
        REQUIRE(strcmp(entry->name, "Fruit") == 0);
        REQUIRE(strcmp(entry->full_name, "FirstNamespace::SecondNamespace::Fruit") == 0);
        REQUIRE(entry->underlying_type_size == sizeof(int));
        REQUIRE(entry->items.size() == 3);
    }
}
