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

TEST_CASE("Enum reflection", "[refl][enum]")
{
    SECTION("DayOfWeek")
    {
        using DayOfWeekEnum = FirstNamespace::DayOfWeek;
        REQUIRE(strcmp(Obs::Enum<DayOfWeekEnum>::GetEnumName(), "DayOfWeek") == 0);
        REQUIRE(strcmp(Obs::Enum<DayOfWeekEnum>::GetFullEnumName(), "FirstNamespace::DayOfWeek") == 0);
        REQUIRE(strcmp(Obs::Enum<DayOfWeekEnum>::GetDescription(), "") == 0);

        REQUIRE(Obs::Enum<DayOfWeekEnum>::GetUnderlyingValue(Obs::Enum<DayOfWeekEnum>::k_end) == 7);

        REQUIRE(strcmp(Obs::Enum<DayOfWeekEnum>::GetValueName(FirstNamespace::Monday), "Monday") == 0);
        REQUIRE(strcmp(Obs::Enum<DayOfWeekEnum>::GetValueName(FirstNamespace::Sunday), "Sunday") == 0);

        REQUIRE(Obs::Enum<DayOfWeekEnum>::GetValue("Monday") == static_cast<DayOfWeekEnum>(0));
        REQUIRE(Obs::Enum<DayOfWeekEnum>::GetValue("Sunday") == static_cast<DayOfWeekEnum>(6));

        REQUIRE(Obs::Enum<DayOfWeekEnum>::GetValue(0) == static_cast<DayOfWeekEnum>(0));
        REQUIRE(Obs::Enum<DayOfWeekEnum>::GetUnderlyingValue(static_cast<DayOfWeekEnum>(0)) == 0);
    }
    SECTION("Vegetable")
    {
        using VegEnum = FirstNamespace::Vegetable;
        REQUIRE(strcmp(Obs::Enum<VegEnum>::GetEnumName(), "Vegetable") == 0);
        REQUIRE(strcmp(Obs::Enum<VegEnum>::GetFullEnumName(), "FirstNamespace::Vegetable") == 0);
        REQUIRE(strcmp(Obs::Enum<VegEnum>::GetDescription(), "This is a vegetable enum.") == 0);

        REQUIRE(Obs::Enum<VegEnum>::GetUnderlyingValue(Obs::Enum<VegEnum>::k_end) == -7);

        REQUIRE(strcmp(Obs::Enum<VegEnum>::GetValueName(VegEnum::Carrot), "Carrot") == 0);
        REQUIRE(strcmp(Obs::Enum<VegEnum>::GetValueName(VegEnum::Potato), "Potato") == 0);
        REQUIRE(strcmp(Obs::Enum<VegEnum>::GetValueName(VegEnum::Cucumber), "Cucumber") == 0);

        REQUIRE(strcmp(Obs::Enum<VegEnum>::GetValueDescription(VegEnum::Carrot), "This is carrot.") == 0);
        REQUIRE(strcmp(Obs::Enum<VegEnum>::GetValueDescription(VegEnum::Potato), "") == 0);
        REQUIRE(strcmp(Obs::Enum<VegEnum>::GetValueDescription(VegEnum::Cucumber), "This is cucumber.") == 0);

        REQUIRE(Obs::Enum<VegEnum>::GetValue("Carrot") == VegEnum::Carrot);
        REQUIRE(Obs::Enum<VegEnum>::GetValue("Potato") == VegEnum::Potato);
        REQUIRE(Obs::Enum<VegEnum>::GetValue("Cucumber") == VegEnum::Cucumber);

        REQUIRE(Obs::Enum<VegEnum>::GetValue(-10) == VegEnum::Carrot);
        REQUIRE(Obs::Enum<VegEnum>::GetValue(-9) == VegEnum::Potato);
        REQUIRE(Obs::Enum<VegEnum>::GetValue(-8) == VegEnum::Cucumber);

        REQUIRE(Obs::Enum<VegEnum>::GetUnderlyingValue(VegEnum::Carrot) == -10);
        REQUIRE(Obs::Enum<VegEnum>::GetUnderlyingValue(VegEnum::Potato) == -9);
        REQUIRE(Obs::Enum<VegEnum>::GetUnderlyingValue(VegEnum::Cucumber) == -8);
    }
    SECTION("Fruit")
    {
        REQUIRE(strcmp(Obs::Enum<Fruit>::GetEnumName(), "Fruit") == 0);
        REQUIRE(strcmp(Obs::Enum<Fruit>::GetFullEnumName(), "FirstNamespace::SecondNamespace::Fruit") == 0);
        REQUIRE(strcmp(Obs::Enum<Fruit>::GetDescription(), "This is a fruit enum.") == 0);

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
    SECTION("DataType")
    {
        using DataTypeEnum = DataStruct::DataType;
        REQUIRE(strcmp(Obs::Enum<DataTypeEnum>::GetEnumName(), "DataType") == 0);
        REQUIRE(strcmp(Obs::Enum<DataTypeEnum>::GetFullEnumName(), "FirstNamespace::SecondNamespace::DataStruct::DataType") == 0);
        REQUIRE(strcmp(Obs::Enum<DataTypeEnum>::GetDescription(), "") == 0);

        REQUIRE(Obs::Enum<DataTypeEnum>::GetUnderlyingValue(Obs::Enum<DataTypeEnum>::k_end) == 3);

        REQUIRE(strcmp(Obs::Enum<DataTypeEnum>::GetValueName(DataTypeEnum::A), "A") == 0);
        REQUIRE(strcmp(Obs::Enum<DataTypeEnum>::GetValueName(DataTypeEnum::B), "B") == 0);
        REQUIRE(strcmp(Obs::Enum<DataTypeEnum>::GetValueName(DataTypeEnum::C), "C") == 0);

        REQUIRE(Obs::Enum<DataTypeEnum>::GetValue("A") == DataTypeEnum::A);
        REQUIRE(Obs::Enum<DataTypeEnum>::GetValue("B") == DataTypeEnum::B);
        REQUIRE(Obs::Enum<DataTypeEnum>::GetValue("C") == DataTypeEnum::C);

        REQUIRE(Obs::Enum<DataTypeEnum>::GetValue(0) == DataTypeEnum::A);
        REQUIRE(Obs::Enum<DataTypeEnum>::GetValue(1) == DataTypeEnum::B);
        REQUIRE(Obs::Enum<DataTypeEnum>::GetValue(2) == DataTypeEnum::C);
    }
}

TEST_CASE("Enum attributes", "[refl][enum][attributes]")
{
    SECTION("Vegetable has attributes")
    {
        using VegEnum = FirstNamespace::Vegetable;
        const auto& attrs = Obs::Enum<VegEnum>::GetAttributes();
        REQUIRE(attrs.size() == 1);
        REQUIRE(strcmp(attrs[0].name, "flags") == 0);
        REQUIRE(strcmp(attrs[0].value, "1") == 0);
    }
    SECTION("DayOfWeek has no attributes")
    {
        using DayOfWeekEnum = FirstNamespace::DayOfWeek;
        const auto& attrs = Obs::Enum<DayOfWeekEnum>::GetAttributes();
        REQUIRE(attrs.empty());
    }
    SECTION("HasAttribute and GetAttributeValue")
    {
        using VegEnum = FirstNamespace::Vegetable;
        using DayOfWeekEnum = FirstNamespace::DayOfWeek;

        REQUIRE(Obs::Enum<VegEnum>::HasAttribute("flags"));
        REQUIRE(strcmp(Obs::Enum<VegEnum>::GetAttributeValue("flags"), "1") == 0);

        REQUIRE(!Obs::Enum<DayOfWeekEnum>::HasAttribute("flags"));
        REQUIRE(Obs::Enum<DayOfWeekEnum>::GetAttributeValue("flags") == nullptr);
    }
}

TEST_CASE("Enum collection", "[refl][enum-collection]")
{
    SECTION("Get enum entries")
    {
        const Obs::EnumEntry* entry = nullptr;

        REQUIRE(Obs::EnumCollection::GetEnum("DayOfWeek", entry));
        REQUIRE(strcmp(entry->name, "DayOfWeek") == 0);
        REQUIRE(entry->underlying_type_size == sizeof(int));
        REQUIRE(entry->items.size() == 7);

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
        REQUIRE(static_cast<int>(entry->items[0].value) == 5);
        REQUIRE(strcmp(entry->items[1].name, "Orange") == 0);
        REQUIRE(strcmp(entry->items[1].description, "This is orange.") == 0);
        REQUIRE(static_cast<int>(entry->items[1].value) == 6);
    }
    SECTION("Get value by name")
    {
        FirstNamespace::Vegetable val = FirstNamespace::Vegetable::Carrot;
        REQUIRE(Obs::EnumCollection::GetValue(&val, "Vegetable", "Potato"));
        REQUIRE(val == FirstNamespace::Vegetable::Potato);

        Fruit val2 = Fruit::Apple;
        REQUIRE(Obs::EnumCollection::GetValue(&val2, "Fruit", "Orange"));
        REQUIRE(val2 == Fruit::Orange);
    }
    SECTION("Bad names")
    {
        FirstNamespace::Vegetable val = FirstNamespace::Vegetable::Carrot;
        REQUIRE(!Obs::EnumCollection::GetValue(&val, "BadEnum", "Potato"));
        REQUIRE(!Obs::EnumCollection::GetValue(&val, "Vegetable", "BadItem"));
    }
}

TEST_CASE("Class reflection", "[refl][class]")
{
    SECTION("Name and description")
    {
        REQUIRE(strcmp(Obs::Class<DataStruct>::GetName(), "DataStruct") == 0);
        REQUIRE(strcmp(Obs::Class<DataStruct>::GetScope(), "FirstNamespace::SecondNamespace") == 0);
        REQUIRE(strcmp(Obs::Class<DataStruct>::GetScopedName(), "FirstNamespace::SecondNamespace::DataStruct") == 0);
        REQUIRE(strcmp(Obs::Class<DataStruct>::GetDescription(), "This is a test struct.") == 0);
    }
    SECTION("Properties")
    {
        auto it = Obs::Class<DataStruct>::Get().begin();

        REQUIRE(strcmp(it->name, "a") == 0);
        REQUIRE(strcmp(it->type_name, "int32_t") == 0);
        REQUIRE(it->offset == offsetof(DataStruct, a));
        REQUIRE(it->size == sizeof(int));

        ++it;
        REQUIRE(strcmp(it->name, "b") == 0);
        REQUIRE(strcmp(it->type_name, "float") == 0);
        REQUIRE(it->offset == offsetof(DataStruct, b));
        REQUIRE(it->size == sizeof(float));

        ++it;
        REQUIRE(strcmp(it->name, "c") == 0);
        REQUIRE(it->offset == offsetof(DataStruct, c));
        REQUIRE(it->size == sizeof(const char*));

        ++it;
        REQUIRE(strcmp(it->name, "d") == 0);
        REQUIRE(it->offset == offsetof(DataStruct, d));
        REQUIRE(it->size == sizeof(DataStruct::DataType));

        ++it;
        REQUIRE(strcmp(it->name, "e") == 0);
        REQUIRE(it->offset == offsetof(DataStruct, e));
        REQUIRE(it->size == sizeof(std::string));
    }
    SECTION("Property descriptions")
    {
        auto it = Obs::Class<DataStruct>::Get().begin();
        REQUIRE(strcmp(it->name, "a") == 0);
        REQUIRE(strcmp(it->description, "The first value.") == 0);

        ++it;
        REQUIRE(strcmp(it->name, "b") == 0);
        REQUIRE(strcmp(it->description, "The second value.") == 0);

        ++it;
        REQUIRE(strcmp(it->name, "c") == 0);
        REQUIRE(strcmp(it->description, "") == 0);

        ++it;
        REQUIRE(strcmp(it->name, "d") == 0);
        REQUIRE(strcmp(it->description, "") == 0);

        ++it;
        REQUIRE(strcmp(it->name, "e") == 0);
        REQUIRE(strcmp(it->description, "") == 0);
    }
    SECTION("Read properties")
    {
        DataStruct data;
        data.a = 42;
        data.b = 3.14f;
        data.c = "hello";
        data.d = DataStruct::DataType::B;
        data.e = "world";

        int a_val = 0;
        float b_val = 0.0f;
        const char* c_val = nullptr;
        DataStruct::DataType d_val = DataStruct::DataType::A;
        std::string e_val;

        REQUIRE(Obs::Class<DataStruct>::Read(&a_val, &data, "a"));
        REQUIRE(Obs::Class<DataStruct>::Read(&b_val, &data, "b"));
        REQUIRE(Obs::Class<DataStruct>::Read(&c_val, &data, "c"));
        REQUIRE(Obs::Class<DataStruct>::Read(&d_val, &data, "d"));
        REQUIRE(Obs::Class<DataStruct>::Read(&e_val, &data, "e"));

        REQUIRE(a_val == 42);
        REQUIRE(b_val == 3.14f);
        REQUIRE(strcmp(c_val, "hello") == 0);
        REQUIRE(d_val == DataStruct::DataType::B);
        REQUIRE(e_val == "world");
    }
    SECTION("Write properties")
    {
        DataStruct data;
        data.a = 1;
        data.b = 2.0f;
        data.c = "old";
        data.d = DataStruct::DataType::B;
        data.e = "old";

        int a_val = 0;
        float b_val = 0.0f;
        const char* c_val = nullptr;
        DataStruct::DataType d_val = DataStruct::DataType::A;
        std::string e_val;

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
    SECTION("Bad property name")
    {
        DataStruct data;
        int val = 0;
        REQUIRE(!Obs::Class<DataStruct>::Read(&val, &data, "nonexistent"));
        REQUIRE(!Obs::Class<DataStruct>::Write(&val, &data, "nonexistent"));
    }
}

TEST_CASE("Class collection", "[refl][class-collection]")
{
    SECTION("Get class entry")
    {
        const Obs::ClassEntry* entry = nullptr;
        REQUIRE(Obs::ClassCollection::GetClassEntry("DataStruct", entry));
        REQUIRE(strcmp(entry->name, "DataStruct") == 0);
        REQUIRE(strcmp(entry->scope, "FirstNamespace::SecondNamespace") == 0);
        REQUIRE(strcmp(entry->scoped_name, "FirstNamespace::SecondNamespace::DataStruct") == 0);
        REQUIRE(strcmp(entry->description, "This is a test struct.") == 0);
        REQUIRE(entry->properties.size() == 5);
    }
    SECTION("Bad class name")
    {
        const Obs::ClassEntry* entry = nullptr;
        REQUIRE(!Obs::ClassCollection::GetClassEntry("BadClass", entry));
    }
    SECTION("Get class properties")
    {
        const Obs::ClassEntry* entry = nullptr;
        REQUIRE(Obs::ClassCollection::GetClassEntry("DataStruct", entry));

        const std::vector<Obs::Property>* props = nullptr;
        REQUIRE(Obs::ClassCollection::GetClassProperties(*entry, props));
        REQUIRE(props->size() == 5);
        REQUIRE(strcmp((*props)[0].name, "a") == 0);
        REQUIRE(strcmp((*props)[1].name, "b") == 0);
        REQUIRE(strcmp((*props)[2].name, "c") == 0);
        REQUIRE(strcmp((*props)[3].name, "d") == 0);
        REQUIRE(strcmp((*props)[4].name, "e") == 0);
    }
    SECTION("Get property")
    {
        const Obs::ClassEntry* entry = nullptr;
        REQUIRE(Obs::ClassCollection::GetClassEntry("DataStruct", entry));

        const Obs::Property* prop = nullptr;
        REQUIRE(Obs::ClassCollection::GetProperty(*entry, "a", prop));
        REQUIRE(strcmp(prop->name, "a") == 0);
        REQUIRE(strcmp(prop->type_name, "int32_t") == 0);
        REQUIRE(prop->is_pod == true);
        REQUIRE(prop->offset == offsetof(DataStruct, a));
        REQUIRE(prop->size == sizeof(int32_t));

        REQUIRE(Obs::ClassCollection::GetProperty(*entry, "b", prop));
        REQUIRE(strcmp(prop->name, "b") == 0);
        REQUIRE(strcmp(prop->type_name, "float") == 0);

        REQUIRE(!Obs::ClassCollection::GetProperty(*entry, "nonexistent", prop));
    }
    SECTION("Read properties")
    {
        DataStruct data;
        data.a = 42;
        data.b = 3.14f;
        data.c = "hello";
        data.d = DataStruct::DataType::B;
        data.e = "world";

        int a_val = 0;
        float b_val = 0.0f;
        const char* c_val = nullptr;
        DataStruct::DataType d_val = DataStruct::DataType::A;
        std::string e_val;

        REQUIRE(Obs::ClassCollection::Read(&a_val, &data, "DataStruct", "a"));
        REQUIRE(Obs::ClassCollection::Read(&b_val, &data, "DataStruct", "b"));
        REQUIRE(Obs::ClassCollection::Read(&c_val, &data, "DataStruct", "c"));
        REQUIRE(Obs::ClassCollection::Read(&d_val, &data, "DataStruct", "d"));
        REQUIRE(Obs::ClassCollection::Read(&e_val, &data, "DataStruct", "e"));

        REQUIRE(a_val == 42);
        REQUIRE(b_val == 3.14f);
        REQUIRE(strcmp(c_val, "hello") == 0);
        REQUIRE(d_val == DataStruct::DataType::B);
        REQUIRE(e_val == "world");
    }
    SECTION("Read via Property reference")
    {
        DataStruct data;
        data.a = 99;

        const Obs::ClassEntry* entry = nullptr;
        REQUIRE(Obs::ClassCollection::GetClassEntry("DataStruct", entry));

        const Obs::Property* prop = nullptr;
        REQUIRE(Obs::ClassCollection::GetProperty(*entry, "a", prop));

        int a_val = 0;
        REQUIRE(Obs::ClassCollection::Read(&a_val, &data, *prop));
        REQUIRE(a_val == 99);
    }
    SECTION("Write properties")
    {
        DataStruct data;
        data.a = 1;
        data.b = 2.0f;
        data.c = "old";
        data.d = DataStruct::DataType::B;
        data.e = "old";

        int a_val = 0;
        float b_val = 0.0f;
        const char* c_val = nullptr;
        DataStruct::DataType d_val = DataStruct::DataType::A;
        std::string e_val;

        REQUIRE(Obs::ClassCollection::Write(&a_val, &data, "DataStruct", "a"));
        REQUIRE(Obs::ClassCollection::Write(&b_val, &data, "DataStruct", "b"));
        REQUIRE(Obs::ClassCollection::Write(&c_val, &data, "DataStruct", "c"));
        REQUIRE(Obs::ClassCollection::Write(&d_val, &data, "DataStruct", "d"));
        REQUIRE(Obs::ClassCollection::Write(&e_val, &data, "DataStruct", "e"));

        REQUIRE(data.a == 0);
        REQUIRE(data.b == 0.0f);
        REQUIRE(data.c == nullptr);
        REQUIRE(data.d == DataStruct::DataType::A);
        REQUIRE(data.e == "");
    }
    SECTION("Write via Property reference")
    {
        DataStruct data;
        data.a = 1;

        const Obs::ClassEntry* entry = nullptr;
        REQUIRE(Obs::ClassCollection::GetClassEntry("DataStruct", entry));

        const Obs::Property* prop = nullptr;
        REQUIRE(Obs::ClassCollection::GetProperty(*entry, "a", prop));

        int a_val = 77;
        REQUIRE(Obs::ClassCollection::Write(&a_val, &data, *prop));
        REQUIRE(data.a == 77);
    }
    SECTION("Bad class or property name")
    {
        DataStruct data;
        int val = 0;
        REQUIRE(!Obs::ClassCollection::Read(&val, &data, "BadClass", "a"));
        REQUIRE(!Obs::ClassCollection::Read(&val, &data, "DataStruct", "nonexistent"));
        REQUIRE(!Obs::ClassCollection::Write(&val, &data, "BadClass", "a"));
        REQUIRE(!Obs::ClassCollection::Write(&val, &data, "DataStruct", "nonexistent"));
    }
    SECTION("Null pointer handling")
    {
        DataStruct data;
        int val = 0;
        REQUIRE(!Obs::ClassCollection::Read(nullptr, &data, "DataStruct", "a"));
        REQUIRE(!Obs::ClassCollection::Read(&val, nullptr, "DataStruct", "a"));
        REQUIRE(!Obs::ClassCollection::Write(nullptr, &data, "DataStruct", "a"));
        REQUIRE(!Obs::ClassCollection::Write(&val, nullptr, "DataStruct", "a"));
    }
    SECTION("Entry attributes")
    {
        const Obs::ClassEntry* entry = nullptr;
        REQUIRE(Obs::ClassCollection::GetClassEntry("DataStruct", entry));
        REQUIRE(entry->HasAttribute("serializable"));
        REQUIRE(strcmp(entry->GetAttributeValue("serializable"), "1") == 0);
        REQUIRE(!entry->HasAttribute("nonexistent"));
    }
    SECTION("Property attributes")
    {
        const Obs::ClassEntry* entry = nullptr;
        REQUIRE(Obs::ClassCollection::GetClassEntry("DataStruct", entry));

        const Obs::Property* prop = nullptr;
        REQUIRE(Obs::ClassCollection::GetProperty(*entry, "a", prop));
        REQUIRE(prop->HasAttribute("min"));
        REQUIRE(strcmp(prop->GetAttributeValue("min"), "0") == 0);
        REQUIRE(strcmp(prop->GetAttributeValue("max"), "100") == 0);

        REQUIRE(Obs::ClassCollection::GetProperty(*entry, "b", prop));
        REQUIRE(!prop->HasAttribute("min"));
        REQUIRE(prop->attributes.empty());
    }
}

TEST_CASE("Class attributes", "[refl][class][attributes]")
{
    SECTION("DataStruct has attributes")
    {
        const auto& attrs = Obs::Class<DataStruct>::GetAttributes();
        REQUIRE(attrs.size() == 1);
        REQUIRE(strcmp(attrs[0].name, "serializable") == 0);
        REQUIRE(strcmp(attrs[0].value, "1") == 0);
    }
    SECTION("HasAttribute and GetAttributeValue")
    {
        REQUIRE(Obs::Class<DataStruct>::HasAttribute("serializable"));
        REQUIRE(strcmp(Obs::Class<DataStruct>::GetAttributeValue("serializable"), "1") == 0);

        REQUIRE(!Obs::Class<DataStruct>::HasAttribute("nonexistent"));
        REQUIRE(Obs::Class<DataStruct>::GetAttributeValue("nonexistent") == nullptr);
    }
    SECTION("Property a has attributes")
    {
        auto it = Obs::Class<DataStruct>::Get().begin();
        REQUIRE(strcmp(it->name, "a") == 0);
        REQUIRE(it->attributes.size() == 2);
        REQUIRE(strcmp(it->attributes[0].name, "min") == 0);
        REQUIRE(strcmp(it->attributes[0].value, "0") == 0);
        REQUIRE(strcmp(it->attributes[1].name, "max") == 0);
        REQUIRE(strcmp(it->attributes[1].value, "100") == 0);

        REQUIRE(it->HasAttribute("min"));
        REQUIRE(strcmp(it->GetAttributeValue("min"), "0") == 0);
        REQUIRE(strcmp(it->GetAttributeValue("max"), "100") == 0);
    }
    SECTION("Property b has no attributes")
    {
        auto it = Obs::Class<DataStruct>::Get().begin() + 1;
        REQUIRE(strcmp(it->name, "b") == 0);
        REQUIRE(it->attributes.empty());
    }
}

TEST_CASE("Class (not struct) reflection", "[refl][class]")
{
    using Player = FirstNamespace::SecondNamespace::Player;

    SECTION("Name and description")
    {
        REQUIRE(strcmp(Obs::Class<Player>::GetName(), "Player") == 0);
        REQUIRE(strcmp(Obs::Class<Player>::GetScope(), "FirstNamespace::SecondNamespace") == 0);
        REQUIRE(strcmp(Obs::Class<Player>::GetScopedName(), "FirstNamespace::SecondNamespace::Player") == 0);
        REQUIRE(strcmp(Obs::Class<Player>::GetDescription(), "A player entity.") == 0);
    }
    SECTION("Attributes")
    {
        REQUIRE(Obs::Class<Player>::HasAttribute("entity"));
        REQUIRE(strcmp(Obs::Class<Player>::GetAttributeValue("entity"), "1") == 0);
    }
    SECTION("Properties")
    {
        auto it = Obs::Class<Player>::Get().begin();

        REQUIRE(strcmp(it->name, "name") == 0);
        REQUIRE(strcmp(it->type_name, "string") == 0);
        REQUIRE(strcmp(it->description, "Player name.") == 0);
        REQUIRE(it->is_pod == false);
        REQUIRE(it->offset == offsetof(Player, name));
        REQUIRE(it->size == sizeof(std::string));

        ++it;
        REQUIRE(strcmp(it->name, "health") == 0);
        REQUIRE(strcmp(it->type_name, "int32_t") == 0);
        REQUIRE(it->is_pod == true);
        REQUIRE(it->offset == offsetof(Player, health));
        REQUIRE(it->size == sizeof(int32_t));

        ++it;
        REQUIRE(strcmp(it->name, "speed") == 0);
        REQUIRE(strcmp(it->type_name, "float") == 0);
        REQUIRE(it->is_pod == true);
        REQUIRE(it->offset == offsetof(Player, speed));
        REQUIRE(it->size == sizeof(float));
    }
    SECTION("Read properties")
    {
        Player player;
        player.name = "hero";
        player.health = 75;
        player.speed = 10.0f;

        std::string name_val;
        int32_t health_val = 0;
        float speed_val = 0.0f;

        REQUIRE(Obs::Class<Player>::Read(&name_val, &player, "name"));
        REQUIRE(Obs::Class<Player>::Read(&health_val, &player, "health"));
        REQUIRE(Obs::Class<Player>::Read(&speed_val, &player, "speed"));

        REQUIRE(name_val == "hero");
        REQUIRE(health_val == 75);
        REQUIRE(speed_val == 10.0f);
    }
    SECTION("Write properties")
    {
        Player player;

        std::string name_val = "new_name";
        int32_t health_val = 200;
        float speed_val = 20.0f;

        REQUIRE(Obs::Class<Player>::Write(&name_val, &player, "name"));
        REQUIRE(Obs::Class<Player>::Write(&health_val, &player, "health"));
        REQUIRE(Obs::Class<Player>::Write(&speed_val, &player, "speed"));

        REQUIRE(player.name == "new_name");
        REQUIRE(player.health == 200);
        REQUIRE(player.speed == 20.0f);
    }
    SECTION("ClassCollection lookup")
    {
        const Obs::ClassEntry* entry = nullptr;
        REQUIRE(Obs::ClassCollection::GetClassEntry("Player", entry));
        REQUIRE(strcmp(entry->name, "Player") == 0);
        REQUIRE(strcmp(entry->scope, "FirstNamespace::SecondNamespace") == 0);
        REQUIRE(strcmp(entry->scoped_name, "FirstNamespace::SecondNamespace::Player") == 0);
        REQUIRE(strcmp(entry->description, "A player entity.") == 0);
        REQUIRE(entry->properties.size() == 3);
        REQUIRE(entry->HasAttribute("entity"));
    }
    SECTION("ClassCollection read and write")
    {
        Player player;
        player.health = 50;

        int32_t health_val = 0;
        REQUIRE(Obs::ClassCollection::Read(&health_val, &player, "Player", "health"));
        REQUIRE(health_val == 50);

        int32_t new_health = 999;
        REQUIRE(Obs::ClassCollection::Write(&new_health, &player, "Player", "health"));
        REQUIRE(player.health == 999);
    }
}

TEST_CASE("Global namespace types", "[refl][global]")
{
    SECTION("GlobalColor enum")
    {
        REQUIRE(strcmp(Obs::Enum<GlobalColor>::GetEnumName(), "GlobalColor") == 0);
        REQUIRE(strcmp(Obs::Enum<GlobalColor>::GetFullEnumName(), "::GlobalColor") == 0);
        REQUIRE(strcmp(Obs::Enum<GlobalColor>::GetDescription(), "A global enum.") == 0);

        REQUIRE(strcmp(Obs::Enum<GlobalColor>::GetValueName(GlobalColor::Red), "Red") == 0);
        REQUIRE(strcmp(Obs::Enum<GlobalColor>::GetValueName(GlobalColor::Green), "Green") == 0);
        REQUIRE(strcmp(Obs::Enum<GlobalColor>::GetValueName(GlobalColor::Blue), "Blue") == 0);

        REQUIRE(Obs::Enum<GlobalColor>::GetValue("Red") == GlobalColor::Red);
        REQUIRE(Obs::Enum<GlobalColor>::GetValue("Blue") == GlobalColor::Blue);
    }
    SECTION("GlobalColor in EnumCollection")
    {
        const Obs::EnumEntry* entry = nullptr;
        REQUIRE(Obs::EnumCollection::GetEnum("GlobalColor", entry));
        REQUIRE(strcmp(entry->name, "GlobalColor") == 0);
        REQUIRE(strcmp(entry->full_name, "::GlobalColor") == 0);
        REQUIRE(entry->items.size() == 3);
    }
    SECTION("GlobalPoint class")
    {
        REQUIRE(strcmp(Obs::Class<GlobalPoint>::GetName(), "GlobalPoint") == 0);
        REQUIRE(strcmp(Obs::Class<GlobalPoint>::GetScope(), "") == 0);
        REQUIRE(strcmp(Obs::Class<GlobalPoint>::GetScopedName(), "::GlobalPoint") == 0);

        auto it = Obs::Class<GlobalPoint>::Get().begin();
        REQUIRE(strcmp(it->name, "x") == 0);
        REQUIRE(strcmp(it->type_name, "float") == 0);
        REQUIRE(it->offset == offsetof(GlobalPoint, x));

        ++it;
        REQUIRE(strcmp(it->name, "y") == 0);
        REQUIRE(strcmp(it->type_name, "float") == 0);
        REQUIRE(it->offset == offsetof(GlobalPoint, y));
    }
    SECTION("GlobalPoint read and write")
    {
        GlobalPoint point;
        point.x = 3.0f;
        point.y = 4.0f;

        float x_val = 0.0f;
        float y_val = 0.0f;
        REQUIRE(Obs::Class<GlobalPoint>::Read(&x_val, &point, "x"));
        REQUIRE(Obs::Class<GlobalPoint>::Read(&y_val, &point, "y"));
        REQUIRE(x_val == 3.0f);
        REQUIRE(y_val == 4.0f);

        float new_x = 10.0f;
        REQUIRE(Obs::Class<GlobalPoint>::Write(&new_x, &point, "x"));
        REQUIRE(point.x == 10.0f);
    }
    SECTION("GlobalPoint in ClassCollection")
    {
        const Obs::ClassEntry* entry = nullptr;
        REQUIRE(Obs::ClassCollection::GetClassEntry("GlobalPoint", entry));
        REQUIRE(strcmp(entry->name, "GlobalPoint") == 0);
        REQUIRE(strcmp(entry->scope, "") == 0);
        REQUIRE(strcmp(entry->scoped_name, "::GlobalPoint") == 0);
        REQUIRE(entry->properties.size() == 2);
    }
}

TEST_CASE("Empty types", "[refl][empty]")
{
    SECTION("EmptyEnum compile-time")
    {
        REQUIRE(strcmp(Obs::Enum<EmptyEnum>::GetEnumName(), "EmptyEnum") == 0);
        REQUIRE(strcmp(Obs::Enum<EmptyEnum>::GetFullEnumName(), "::EmptyEnum") == 0);
        REQUIRE(Obs::Enum<EmptyEnum>::GetAttributes().empty());
    }
    SECTION("EmptyEnum in collection")
    {
        const Obs::EnumEntry* entry = nullptr;
        REQUIRE(Obs::EnumCollection::GetEnum("EmptyEnum", entry));
        REQUIRE(entry->items.empty());
    }
    SECTION("EmptyStruct compile-time")
    {
        REQUIRE(strcmp(Obs::Class<EmptyStruct>::GetName(), "EmptyStruct") == 0);
        REQUIRE(strcmp(Obs::Class<EmptyStruct>::GetScopedName(), "::EmptyStruct") == 0);
        REQUIRE(Obs::Class<EmptyStruct>::GetAttributes().empty());

        auto& instance = Obs::Class<EmptyStruct>::Get();
        REQUIRE(instance.begin() == instance.end());
    }
    SECTION("EmptyStruct in collection")
    {
        const Obs::ClassEntry* entry = nullptr;
        REQUIRE(Obs::ClassCollection::GetClassEntry("EmptyStruct", entry));
        REQUIRE(entry->properties.empty());
    }
}

TEST_CASE("Malformed attributes", "[refl][attributes][malformed]")
{
    SECTION("Enum with edge-case attributes")
    {
        // OBS_ENUM("", "multi=a=b", "standalone")
        // Empty and multiple '=' attributes are skipped, only "standalone" remains
        const auto& attrs = Obs::Enum<MalformedAttrEnum>::GetAttributes();
        REQUIRE(attrs.size() == 1);

        // Key-only: value defaults to "1"
        REQUIRE(strcmp(attrs[0].name, "standalone") == 0);
        REQUIRE(strcmp(attrs[0].value, "1") == 0);
    }
    SECTION("Enum edge-case HasAttribute and GetAttributeValue")
    {
        // Malformed attributes are skipped
        REQUIRE(!Obs::Enum<MalformedAttrEnum>::HasAttribute("multi"));

        REQUIRE(Obs::Enum<MalformedAttrEnum>::HasAttribute("standalone"));
        REQUIRE(strcmp(Obs::Enum<MalformedAttrEnum>::GetAttributeValue("standalone"), "1") == 0);

        REQUIRE(!Obs::Enum<MalformedAttrEnum>::HasAttribute("nonexistent"));
    }
    SECTION("Class with edge-case attributes")
    {
        // OBS_CLASS("=", "valid=yes")
        // "=" is skipped (empty name), only "valid=yes" remains
        const auto& attrs = Obs::Class<MalformedAttrStruct>::GetAttributes();
        REQUIRE(attrs.size() == 1);

        REQUIRE(strcmp(attrs[0].name, "valid") == 0);
        REQUIRE(strcmp(attrs[0].value, "yes") == 0);
    }
    SECTION("Property with empty string attribute")
    {
        // OBS_PROP("") - empty attribute is skipped
        auto it = Obs::Class<MalformedAttrStruct>::Get().begin();
        REQUIRE(strcmp(it->name, "x") == 0);
        REQUIRE(it->attributes.size() == 0);
    }
    SECTION("Property with multiple '=' and key-only attribute")
    {
        // OBS_PROP("a=b=c", "simple")
        // "a=b=c" is skipped (multiple '='), only "simple" remains
        auto it = Obs::Class<MalformedAttrStruct>::Get().begin() + 1;
        REQUIRE(strcmp(it->name, "y") == 0);
        REQUIRE(it->attributes.size() == 1);

        // "simple": key-only, value defaults to "1"
        REQUIRE(strcmp(it->attributes[0].name, "simple") == 0);
        REQUIRE(strcmp(it->attributes[0].value, "1") == 0);
    }
    SECTION("Enum collection edge-case attributes")
    {
        const Obs::EnumEntry* entry = nullptr;
        REQUIRE(Obs::EnumCollection::GetEnum("MalformedAttrEnum", entry));
        REQUIRE(entry->attributes.size() == 1);
        REQUIRE(!entry->HasAttribute("multi"));
        REQUIRE(entry->HasAttribute("standalone"));
    }
    SECTION("Class collection edge-case attributes")
    {
        const Obs::ClassEntry* entry = nullptr;
        REQUIRE(Obs::ClassCollection::GetClassEntry("MalformedAttrStruct", entry));
        REQUIRE(entry->HasAttribute("valid"));
        REQUIRE(strcmp(entry->GetAttributeValue("valid"), "yes") == 0);

        const Obs::Property* prop = nullptr;
        REQUIRE(Obs::ClassCollection::GetProperty(*entry, "y", prop));
        REQUIRE(!prop->HasAttribute("a"));
        REQUIRE(prop->HasAttribute("simple"));
        REQUIRE(strcmp(prop->GetAttributeValue("simple"), "1") == 0);
    }
}

TEST_CASE("String escaping", "[refl][escaping]")
{
    SECTION("Enum with double quotes in description")
    {
        REQUIRE(strcmp(Obs::Enum<QuotedDescEnum>::GetEnumName(), "QuotedDescEnum") == 0);
        REQUIRE(strcmp(Obs::Enum<QuotedDescEnum>::GetDescription(), "The \"important\" enum.") == 0);
        REQUIRE(strcmp(Obs::Enum<QuotedDescEnum>::GetValueDescription(QuotedDescEnum::First), "The \"first\" value.") == 0);
        REQUIRE(strcmp(Obs::Enum<QuotedDescEnum>::GetValueDescription(QuotedDescEnum::Second), "") == 0);
    }
    SECTION("Class with backslashes in description")
    {
        REQUIRE(strcmp(Obs::Class<BackslashDesc>::GetName(), "BackslashDesc") == 0);
        REQUIRE(strcmp(Obs::Class<BackslashDesc>::GetDescription(), "Separator: \\ value.") == 0);

        auto it = Obs::Class<BackslashDesc>::Get().begin();
        REQUIRE(strcmp(it->name, "value") == 0);
        REQUIRE(strcmp(it->description, "Offset: 0x5C \\ backslash.") == 0);
    }
    SECTION("QuotedDescEnum in EnumCollection")
    {
        const Obs::EnumEntry* entry = nullptr;
        REQUIRE(Obs::EnumCollection::GetEnum("QuotedDescEnum", entry));
        REQUIRE(strcmp(entry->description, "The \"important\" enum.") == 0);
        REQUIRE(entry->items.size() == 2);
        REQUIRE(strcmp(entry->items[0].description, "The \"first\" value.") == 0);
    }
    SECTION("BackslashDesc in ClassCollection")
    {
        const Obs::ClassEntry* entry = nullptr;
        REQUIRE(Obs::ClassCollection::GetClassEntry("BackslashDesc", entry));
        REQUIRE(strcmp(entry->description, "Separator: \\ value.") == 0);
        REQUIRE(entry->properties.size() == 1);

        const Obs::Property* prop = nullptr;
        REQUIRE(Obs::ClassCollection::GetProperty(*entry, "value", prop));
        REQUIRE(strcmp(prop->description, "Offset: 0x5C \\ backslash.") == 0);
    }
}
