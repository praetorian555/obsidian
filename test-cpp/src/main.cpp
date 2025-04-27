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

TEST_CASE("Strong type enum reflection", "[refl][enum]")
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

        REQUIRE(strcmp(Obs::Enum<Fruit>::GetValueDescription(Fruit::Apple), "") == 0);
        REQUIRE(strcmp(Obs::Enum<Fruit>::GetValueDescription(Fruit::Orange), "This is orange.") == 0);
        REQUIRE(strcmp(Obs::Enum<Fruit>::GetValueDescription(Fruit::Banana), "") == 0);

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

        REQUIRE(strcmp(Obs::Enum<FirstNamespace::Vegetable>::GetValueDescription(FirstNamespace::Vegetable::Carrot), "This is carrot.") == 0);
        REQUIRE(strcmp(Obs::Enum<FirstNamespace::Vegetable>::GetValueDescription(FirstNamespace::Vegetable::Potato), "") == 0);
        REQUIRE(strcmp(Obs::Enum<FirstNamespace::Vegetable>::GetValueDescription(FirstNamespace::Vegetable::Cucumber), "This is cucumber.") == 0);

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
        SECTION("DataType") // Added section for DataType
    {
        using DataTypeEnum = FirstNamespace::SecondNamespace::DataStruct::DataType; // Alias for brevity
        std::string enum_name = Obs::Enum<DataTypeEnum>::GetEnumName();
        REQUIRE(enum_name == "DataType");
        std::string full_enum_name = Obs::Enum<DataTypeEnum>::GetFullEnumName();
        REQUIRE(full_enum_name == "FirstNamespace::SecondNamespace::DataStruct::DataType");
        std::string description = Obs::Enum<DataTypeEnum>::GetDescription();
        REQUIRE(description == ""); // No description provided

        REQUIRE(Obs::Enum<DataTypeEnum>::GetUnderlyingValue(Obs::Enum<DataTypeEnum>::k_end) == 3); // Default enum values end at C=2, so k_end is 3

        REQUIRE(strcmp(Obs::Enum<DataTypeEnum>::GetValueName(DataTypeEnum::A), "A") == 0);
        REQUIRE(strcmp(Obs::Enum<DataTypeEnum>::GetValueName(DataTypeEnum::B), "B") == 0);
        REQUIRE(strcmp(Obs::Enum<DataTypeEnum>::GetValueName(DataTypeEnum::C), "C") == 0);

        // Assuming no descriptions for individual values
        REQUIRE(strcmp(Obs::Enum<DataTypeEnum>::GetValueDescription(DataTypeEnum::A), "") == 0);
        REQUIRE(strcmp(Obs::Enum<DataTypeEnum>::GetValueDescription(DataTypeEnum::B), "") == 0);
        REQUIRE(strcmp(Obs::Enum<DataTypeEnum>::GetValueDescription(DataTypeEnum::C), "") == 0);

        REQUIRE(Obs::Enum<DataTypeEnum>::GetValue("A") == DataTypeEnum::A);
        REQUIRE(Obs::Enum<DataTypeEnum>::GetValue("B") == DataTypeEnum::B);
        REQUIRE(Obs::Enum<DataTypeEnum>::GetValue("C") == DataTypeEnum::C);

        // Default underlying values: A=0, B=1, C=2
        REQUIRE(Obs::Enum<DataTypeEnum>::GetValue(0) == DataTypeEnum::A);
        REQUIRE(Obs::Enum<DataTypeEnum>::GetValue(1) == DataTypeEnum::B);
        REQUIRE(Obs::Enum<DataTypeEnum>::GetValue(2) == DataTypeEnum::C);

        REQUIRE(Obs::Enum<DataTypeEnum>::GetUnderlyingValue(DataTypeEnum::A) == 0);
        REQUIRE(Obs::Enum<DataTypeEnum>::GetUnderlyingValue(DataTypeEnum::B) == 1);
        REQUIRE(Obs::Enum<DataTypeEnum>::GetUnderlyingValue(DataTypeEnum::C) == 2);
    }

}

TEST_CASE("Enum collection", "[refl][enum]")
{
    Obs::EnumCollection& collection = Obs::EnumCollection::Get();
    SECTION("Collection info")
    {
        REQUIRE(collection.entries.size() == 3);
        REQUIRE(strcmp(collection.entries[0].name, "Vegetable") == 0);
        REQUIRE(strcmp(collection.entries[0].full_name, "FirstNamespace::Vegetable") == 0);
        REQUIRE(strcmp(collection.entries[0].description, "This is a vegetable enum.") == 0);
        REQUIRE(collection.entries[0].underlying_type_size == 1);
        REQUIRE(collection.entries[0].items.size() == 3);
        REQUIRE(strcmp(collection.entries[1].name, "Fruit") == 0);
        REQUIRE(strcmp(collection.entries[1].full_name, "FirstNamespace::SecondNamespace::Fruit") == 0);
        REQUIRE(strcmp(collection.entries[1].description, "This is a fruit enum.") == 0);
        REQUIRE(collection.entries[1].underlying_type_size == sizeof(int));
        REQUIRE(collection.entries[1].items.size() == 3);
        REQUIRE(strcmp(collection.entries[2].name, "DataType") == 0);
        REQUIRE(strcmp(collection.entries[2].full_name, "FirstNamespace::SecondNamespace::DataStruct::DataType") == 0);
        REQUIRE(strcmp(collection.entries[2].description, "") == 0);
        REQUIRE(collection.entries[2].underlying_type_size == 2);
        REQUIRE(collection.entries[2].items.size() == 3);
    }
    SECTION("Collection items")
    {
        REQUIRE(strcmp(collection.entries[0].items[0].name, "Carrot") == 0);
        REQUIRE(static_cast<int8_t>(collection.entries[0].items[0].value) == -10);
        REQUIRE(strcmp(collection.entries[0].items[1].name, "Potato") == 0);
        REQUIRE(static_cast<int8_t>(collection.entries[0].items[1].value) == -9);
        REQUIRE(strcmp(collection.entries[0].items[2].name, "Cucumber") == 0);
        REQUIRE(static_cast<int8_t>(collection.entries[0].items[2].value) == -8);

        REQUIRE(strcmp(collection.entries[1].items[0].name, "Apple") == 0);
        REQUIRE(strcmp(collection.entries[1].items[0].description, "") == 0);
        REQUIRE(static_cast<int>(collection.entries[1].items[0].value) == 5);
        REQUIRE(strcmp(collection.entries[1].items[1].name, "Orange") == 0);
        REQUIRE(strcmp(collection.entries[1].items[1].description, "This is orange.") == 0);
        REQUIRE(static_cast<int>(collection.entries[1].items[1].value) == 6);
        REQUIRE(strcmp(collection.entries[1].items[2].name, "Banana") == 0);
        REQUIRE(static_cast<int>(collection.entries[1].items[2].value) == 7);
    }
    SECTION("Collection get item value by name")
    {
        FirstNamespace::Vegetable val = FirstNamespace::Vegetable::Carrot;
        collection.GetValue(&val, "Vegetable", "Potato");
        REQUIRE(val == FirstNamespace::Vegetable::Potato);

        Fruit val2 = Fruit::Apple;
        collection.GetValue(&val2, "Fruit", "Orange");
        REQUIRE(val2 == Fruit::Orange);
    }
    SECTION("Collection get enum entry by name")
    {
        auto entry = collection.GetEnumEntry("Fruit");
        REQUIRE(strcmp(entry.name, "Fruit") == 0);
        REQUIRE(strcmp(entry.full_name, "FirstNamespace::SecondNamespace::Fruit") == 0);
        REQUIRE(entry.underlying_type_size == sizeof(int));
        REQUIRE(entry.items.size() == 3);
    }
}
