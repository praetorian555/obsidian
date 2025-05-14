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
    SECTION("Bad enum name")
    {
        const Obs::EnumEntry* entry = nullptr;
        REQUIRE(!Obs::EnumCollection::GetEnum("BadEnum", entry));
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
    SECTION("Try to get item value with bad enum name")
    {
        FirstNamespace::Vegetable val = FirstNamespace::Vegetable::Carrot;
        REQUIRE(!Obs::EnumCollection::GetValue(&val, "BadEnum", "Potato"));
    }
    SECTION("Try to get item value with bad item name")
    {
        FirstNamespace::Vegetable val = FirstNamespace::Vegetable::Carrot;
        REQUIRE(!Obs::EnumCollection::GetValue(&val, "Vegetable", "BadItem"));
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

TEST_CASE("Struct compile-time reflection", "[refl][struct]")
{
    SECTION("Check name, scope and description")
    {
        REQUIRE(strcmp(Obs::Class<DataStruct>::GetName(), "DataStruct") == 0);
        REQUIRE(strcmp(Obs::Class<DataStruct>::GetScope(), "FirstNamespace::SecondNamespace") == 0);
        REQUIRE(strcmp(Obs::Class<DataStruct>::GetScopedName(), "FirstNamespace::SecondNamespace::DataStruct") == 0);
        REQUIRE(strcmp(Obs::Class<DataStruct>::GetDescription(), "This is a test struct.") == 0);
    }
    SECTION("Check properties")
    {
        const Obs::Property& first_prop = *Obs::Class<DataStruct>::Get().begin();
        REQUIRE(strcmp(first_prop.name, "a") == 0);
        REQUIRE(strcmp(first_prop.type_name, "int") == 0);
        REQUIRE(first_prop.offset == offsetof(DataStruct, a));
        REQUIRE(first_prop.size == sizeof(int));

        const Obs::Property& second_prop = *(Obs::Class<DataStruct>::Get().begin() + 1);
        REQUIRE(strcmp(second_prop.name, "b") == 0);
        REQUIRE(strcmp(second_prop.type_name, "float") == 0);
        REQUIRE(second_prop.offset == offsetof(DataStruct, b));
        REQUIRE(second_prop.size == sizeof(float));

        const Obs::Property& third_prop = *(Obs::Class<DataStruct>::Get().begin() + 2);
        REQUIRE(strcmp(third_prop.name, "c") == 0);
        REQUIRE(strcmp(third_prop.type_name, "char const *") == 0);
        REQUIRE(third_prop.offset == offsetof(DataStruct, c));
        REQUIRE(third_prop.size == sizeof(char const *));

        const Obs::Property& fourth_prop = *(Obs::Class<DataStruct>::Get().begin() + 3);
        REQUIRE(strcmp(fourth_prop.name, "d") == 0);
        REQUIRE(strcmp(fourth_prop.type_name, "FirstNamespace::SecondNamespace::DataStruct::DataType") == 0);
        REQUIRE(fourth_prop.offset == offsetof(DataStruct, d));
        REQUIRE(fourth_prop.size == sizeof(FirstNamespace::SecondNamespace::DataStruct::DataType));

        const Obs::Property& fifth_prop = *(Obs::Class<DataStruct>::Get().begin() + 4);
        REQUIRE(strcmp(fifth_prop.name, "e") == 0);
        REQUIRE(strcmp(fifth_prop.type_name, "std::basic_string<char, std::char_traits<char>, std::allocator<char>>") == 0);
        REQUIRE(fifth_prop.offset == offsetof(DataStruct, e));
        REQUIRE(fifth_prop.size == sizeof(std::string));
    }
    SECTION("Read property values")
    {
        DataStruct data;
        data.a = 1;
        data.b = 2.0f;
        data.c = "Hello world!";
        data.d = DataStruct::DataType::B;
        data.e = "Hello world!";

        int a_val = 0;
        float b_val = 0.0f;
        char const* c_val = nullptr;
        DataStruct::DataType d_val = DataStruct::DataType::A;
        std::string e_val = "";

        REQUIRE(Obs::Class<DataStruct>::Read(&a_val, &data, "a"));
        REQUIRE(Obs::Class<DataStruct>::Read(&b_val, &data, "b"));
        REQUIRE(Obs::Class<DataStruct>::Read(&c_val, &data, "c"));
        REQUIRE(Obs::Class<DataStruct>::Read(&d_val, &data, "d"));
        REQUIRE(Obs::Class<DataStruct>::Read(&e_val, &data, "e"));

        REQUIRE(a_val == 1);
        REQUIRE(b_val == 2.0f);
        REQUIRE(strcmp(c_val, "Hello world!") == 0);
        REQUIRE(d_val == DataStruct::DataType::B);
        REQUIRE(strcmp(e_val.c_str(), "Hello world!") == 0);
    }
    SECTION("Try read with bad property name")
    {
        DataStruct data;
        data.a = 1;
        int a_val = 0;

        REQUIRE(!Obs::Class<DataStruct>::Read(&a_val, &data, "bad_prop"));
        REQUIRE(a_val == 0);

    }
    SECTION("Write property values")
    {
        DataStruct data;
        data.a = 1;
        data.b = 2.0f;
        data.c = "Hello world!";
        data.d = DataStruct::DataType::B;
        data.e = "Hello world!";

        int a_val = 0;
        float b_val = 0.0f;
        char const* c_val = nullptr;
        DataStruct::DataType d_val = DataStruct::DataType::A;
        std::string e_val = "";

        REQUIRE(Obs::Class<DataStruct>::Write(&a_val, &data, "a"));
        REQUIRE(Obs::Class<DataStruct>::Write(&b_val, &data, "b"));
        REQUIRE(Obs::Class<DataStruct>::Write(&c_val, &data, "c"));
        REQUIRE(Obs::Class<DataStruct>::Write(&d_val, &data, "d"));
        REQUIRE(Obs::Class<DataStruct>::Write(&e_val, &data, "e"));

        REQUIRE(data.a == 0);
        REQUIRE(data.b == 0.0f);
        REQUIRE(data.c == nullptr);
        REQUIRE(data.d == DataStruct::DataType::A);
        REQUIRE(data.e == "");
    }
    SECTION("Try write with bad property name")
    {
        DataStruct data;
        data.a = 1;
        int a_val = 0;

        REQUIRE(!Obs::Class<DataStruct>::Write(&a_val, &data, "bad_prop"));
        REQUIRE(data.a == 1);
    }
}