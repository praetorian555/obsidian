#pragma once

namespace ObsTemplates
{

constexpr const char* k_reflection_header_template = R"(// AUTO-GENERATED. DO NOT CHANGE.

#pragma once

#include <cstring>
#include <vector>
#include <string>

__refl_includes__

namespace Obs
{

#pragma region Compile-Time Enum Reflection

template <typename T>
struct Enum
{
    static_assert(false, "Enum type does not have reflection data!");
};

__refl_enum__

#pragma endregion

struct EnumItem
{
    const char* name = "";
    const char* description = "";
    uint64_t value = 0xFFFFFFFFFFFFFFFF;
};

struct EnumEntry
{
    const char* name = "";
    const char* full_name = "";
    const char* description = "";
    int underlying_type_size = 0;
    std::vector<EnumItem> items;
};

#pragma region Run-Time Enum Reflection

__refl_enum_collection__

#pragma endregion

enum class Type
{
    POD,
    Pointer,
    String
};

struct Property
{
    const char* name;
    const char* description;
    const char* type_name;
    Type type_enum;
    int offset;
    int size;
};

struct ClassEntry
{
    const char* name;
    const char* scope;
    const char* scoped_name;
    const char* description;

    std::vector<Property> properties;
};

#pragma region Compile-Time Class Reflection

template <typename T>
struct Class
{
    static_assert(false, "Class type does not have reflection data!");
};

__refl_class__

#pragma endregion

#pragma region Run-Time Class Reflection

__refl_class_collection__

#pragma endregion

} // namespace Obs
)";

constexpr const char* k_enum_template = R"(template <>
struct Enum<__enum_full_name__>
{
    using UnderlyingType = int;
    using EnumType = __enum_full_name__;
    static constexpr EnumType k_end = static_cast<EnumType>(static_cast<UnderlyingType>(__enum_last_entry__) + 1);

    static const char* GetFullEnumName() { return "__enum_full_name__"; }
    static const char* GetEnumName() { return "__enum_name__"; }
    static const char* GetDescription() { return "__enum_comment__"; }

    static const char* GetValueDescription(EnumType enum_value)
    {
        switch (enum_value)
        {
__enum_value_to_description_switch__
            default: return nullptr;
        }
    }

    static UnderlyingType GetUnderlyingValue(EnumType enum_value)
    {
        return static_cast<UnderlyingType>(enum_value);
    }

    static EnumType GetValue(UnderlyingType value)
    {
        return static_cast<EnumType>(value);
    }

    static const char* GetValueName(EnumType enum_value)
    {
        switch (enum_value)
        {
__enum_value_to_name_switch__
            default: return nullptr;
        }
    }

    static EnumType GetValue(const char* name)
    {
__enum_name_to_value_switch__
        return k_end;
    }
};
)";

constexpr const char* k_class_template = R"(template <>
struct Class<__class_scoped_name__>
{
	static const char* GetName() { return "__class_name__"; }
	static const char* GetScope() { return "__class_scope__"; }
	static const char* GetScopedName() { return "__class_scoped_name__"; }
	static const char* GetDescription() { return "__class_description__"; }

	static Class& Get()
	{
		static Class s_instance;
		return s_instance;
	}

	struct ConstIterator
	{
		ConstIterator(const Class* cls, int position) : m_class(cls), m_pos(position) {}
		bool operator==(const ConstIterator& other) const { return m_class == other.m_class && m_pos == other.m_pos; }
		const Property& operator*() const { return m_class->m_properties[m_pos]; }
        const Property* operator->() const { return &m_class->m_properties[m_pos]; }
        ConstIterator operator++(int) { ConstIterator tmp = *this; m_pos++; return tmp; }
        ConstIterator& operator++() { m_pos++; return *this; }

        ConstIterator operator+(int n) const { return ConstIterator(m_class, m_pos + n); }
        ConstIterator operator-(int n) const { return ConstIterator(m_class, m_pos - n); }

	private:
		const Class* m_class;
		int m_pos;
	};

    ConstIterator begin() const { return ConstIterator(this, 0); }
    ConstIterator end() const { return ConstIterator(this, this->m_properties.size()); }

	static bool Read(void* out_value, void* object, const char* property_name)
    {
        if (object == nullptr || out_value == nullptr || property_name == nullptr)
        {
            return false;
        }
        for (auto& prop : Get())
        {
            if (strcmp(prop.name, property_name) == 0)
            {
                if (prop.type_enum == Type::String)
                {
                    std::string& out = *reinterpret_cast<std::string*>(out_value);
                    std::string& in = *reinterpret_cast<std::string*>((char*)object + prop.offset);
                    out = in;
                }
                else
                {
                    memcpy(out_value, (char*)object + prop.offset, prop.size);
                }
                return true;
            }
        }
        return false;
    }

    static bool Write(void* value, void* object, const char* property_name)
    {
        if (object == nullptr || value == nullptr || property_name == nullptr)
        {
            return false;
        }
        for (auto& prop : Get())
        {

            if (strcmp(prop.name, property_name) == 0)
            {
                if (prop.type_enum == Type::String)
                {
                    std::string& in = *reinterpret_cast<std::string*>(value);
                    std::string& out = *reinterpret_cast<std::string*>((char*)object + prop.offset);
                    out = in;
                }
                else
                {
                    memcpy((char*)object + prop.offset, value, prop.size);
                }
                return true;
            }
        }
        return false;
    }

private:
	std::vector<Property> m_properties = __class_init_properties__;
};
)";

constexpr const char* k_enum_collection_template = R"(struct EnumCollection
{
    static bool GetEnum(const char* enum_name, const EnumEntry*& out_entry)
    {
        for (EnumEntry& entry : s_entries)
        {
            if (strcmp(entry.name, enum_name) == 0)
            {
                out_entry = &entry;
                return true;
            }
        }
        return false;
    }

    static bool GetValue(void* out_value, const char* enum_name, const char* item_name)
    {
        for (EnumEntry& enum_entry : s_entries)
        {
            if (strcmp(enum_entry.name, enum_name) == 0)
            {
                for (EnumItem& item : enum_entry.items)
                {
                    if (strcmp(item.name, item_name) == 0)
                    {
                        memcpy(out_value, reinterpret_cast<const void*>(&item.value), enum_entry.underlying_type_size);
                        return true;
                    }
                }
            }
        }
        return false;
    }

private:
    static inline std::vector<EnumEntry> s_entries = __enum_collection_entries__;
};
)";

constexpr const char* k_class_collection_template = R"(struct ClassCollection
{
    static bool GetClassEntry(const char* name, const ClassEntry*& out_entry)
    {
        for (const ClassEntry& entry : entries)
        {
            if (strcmp(entry.name, name) == 0)
            {
                out_entry = &entry;
                return true;
            }
        }
        return false;
    }

    static bool GetClassProperties(const ClassEntry& class_entry, const std::vector<Property>*& out_properties)
    {
        out_properties = &class_entry.properties;
        return true;
    }

    static bool GetProperty(const ClassEntry& class_entry, const char* property_name, const Property*& out_prop)
    {
        for (const Property& prop : class_entry.properties)
        {
            if (strcmp(prop.name, property_name) == 0)
            {
                out_prop = &prop;
                return true;
            }
        }
        return false;
    }

    static bool Read(void* out_value, void* object, const char* class_name, const char* property_name)
    {
        if (object == nullptr || out_value == nullptr || class_name == nullptr || property_name == nullptr)
        {
            return false;
        }
        for (const ClassEntry& class_entry : entries)
        {
            if (strcmp(class_entry.name, class_name) != 0)
            {
                continue;
            }
            for (const Property& prop : class_entry.properties)
            {
                if (strcmp(prop.name, property_name) == 0)
                {
                    return Read(out_value, object, prop);
                }
            }
        }
        return false;
    }

    static bool Read(void* out_value, void* object, const Property& prop)
    {
        if (object == nullptr || out_value == nullptr)
        {
            return false;
        }
        if (prop.type_enum == Type::String)
        {
            std::string& out = *reinterpret_cast<std::string*>(out_value);
            std::string& in = *reinterpret_cast<std::string*>((char*)object + prop.offset);
            out = in;
        }
        else
        {
            memcpy(out_value, (char*)object + prop.offset, prop.size);
        }
        return true;
    }

    static bool Write(void* value, void* object, const char* class_name, const char* property_name)
    {
        if (object == nullptr || value == nullptr || class_name == nullptr || property_name == nullptr)
        {
            return false;
        }
        for (const ClassEntry& class_entry : entries)
        {
            if (strcmp(class_entry.name, class_name) != 0)
            {
                continue;
            }
            for (const Property& prop : class_entry.properties)
            {
                if (strcmp(prop.name, property_name) == 0)
                {
                    return Write(value, object, prop);
                }
            }
        }
        return false;
    }

    static bool Write(void* value, void* object, const Property& prop)
    {
        if (object == nullptr || value == nullptr)
        {
            return false;
        }
        if (prop.type_enum == Type::String)
        {
            std::string& in = *reinterpret_cast<std::string*>(value);
            std::string& out = *reinterpret_cast<std::string*>((char*)object + prop.offset);
            out = in;
        }
        else
        {
            memcpy((char*)object + prop.offset, value, prop.size);
        }
        return true;
    }

private:
    static inline const std::vector<ClassEntry> entries = __class_collection_entries__;
};
)";

constexpr const char* k_obs_header = R"(#pragma once

#define OBS_ENUM(...)
#define OBS_CLASS(...)
#define OBS_PROP(...)
)";

} // namespace ObsTemplates