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
