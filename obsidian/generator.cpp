#include "generator.hpp"
#include "types.hpp"
#include "templates.hpp"

#include <cstdio>
#include <cstring>

#include "opal/paths.h"
#include "opal/file-system.h"

static Opal::StringUtf8 ReplaceAll(const Opal::StringUtf8& source, const char* placeholder, const Opal::StringUtf8& replacement)
{
    Opal::StringUtf8 result = source;
    const Opal::u64 placeholder_len = strlen(placeholder);
    Opal::u64 pos = Opal::Find<Opal::StringUtf8>(result, placeholder);
    while (pos != Opal::StringUtf8::k_npos)
    {
        Opal::StringUtf8 prefix = Opal::GetSubString(result, 0, pos).GetValue();
        Opal::StringUtf8 suffix;
        if (pos + placeholder_len < result.GetSize())
        {
            suffix = Opal::GetSubString(result, pos + placeholder_len).GetValue();
        }
        result = prefix + replacement + suffix;
        pos = Opal::Find<Opal::StringUtf8>(result, placeholder, pos + replacement.GetSize());
    }
    return result;
}

static Opal::StringUtf8 IntToString(Opal::i64 value)
{
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%lld", value);
    return Opal::StringUtf8(buffer);
}

static bool WriteToFile(const Opal::StringUtf8& path, const Opal::StringUtf8& content)
{
    FILE* file = fopen(path.GetData(), "w");
    if (file == nullptr)
    {
        return false;
    }
    fwrite(content.GetData(), 1, content.GetSize(), file);
    fclose(file);
    return true;
}

static Opal::StringUtf8 QualifiedConstantName(const CppEnum& cpp_enum, const CppEnumConstant& constant)
{
    if (cpp_enum.is_enum_class)
    {
        return cpp_enum.full_name + "::" + constant.name;
    }
    if (!cpp_enum.scope.IsEmpty())
    {
        return cpp_enum.scope + "::" + constant.name;
    }
    return constant.name;
}

static Opal::StringUtf8 GenerateEnumSpecialization(const CppEnum& cpp_enum)
{
    Opal::StringUtf8 result = ObsTemplates::k_enum_template;

    result = ReplaceAll(result, "__enum_full_name__", cpp_enum.full_name);
    result = ReplaceAll(result, "__enum_name__", cpp_enum.name);
    result = ReplaceAll(result, "__enum_comment__", cpp_enum.description);

    // Last entry
    if (!cpp_enum.constants.IsEmpty())
    {
        const CppEnumConstant& last = cpp_enum.constants[cpp_enum.constants.GetSize() - 1];
        result = ReplaceAll(result, "__enum_last_entry__", QualifiedConstantName(cpp_enum, last));
    }

    // Value to description switch
    Opal::StringUtf8 desc_switch;
    for (Opal::u64 i = 0; i < cpp_enum.constants.GetSize(); i++)
    {
        if (i > 0)
        {
            desc_switch += "\n";
        }
        desc_switch += "            case " + QualifiedConstantName(cpp_enum, cpp_enum.constants[i]) + ": return \""
                       + cpp_enum.constants[i].description + "\";";
    }
    result = ReplaceAll(result, "__enum_value_to_description_switch__", desc_switch);

    // Value to name switch
    Opal::StringUtf8 name_switch;
    for (Opal::u64 i = 0; i < cpp_enum.constants.GetSize(); i++)
    {
        if (i > 0)
        {
            name_switch += "\n";
        }
        name_switch += "            case " + QualifiedConstantName(cpp_enum, cpp_enum.constants[i]) + ": return \""
                       + cpp_enum.constants[i].name + "\";";
    }
    result = ReplaceAll(result, "__enum_value_to_name_switch__", name_switch);

    // Name to value switch
    Opal::StringUtf8 name_to_value;
    for (Opal::u64 i = 0; i < cpp_enum.constants.GetSize(); i++)
    {
        if (i > 0)
        {
            name_to_value += "\n";
        }
        name_to_value += "        if (strcmp(name, \"" + cpp_enum.constants[i].name + "\") == 0) return "
                         + QualifiedConstantName(cpp_enum, cpp_enum.constants[i]) + ";";
    }
    result = ReplaceAll(result, "__enum_name_to_value_switch__", name_to_value);

    return result;
}

static Opal::StringUtf8 GenerateClassSpecialization(const CppClass& cpp_class)
{
    Opal::StringUtf8 result = ObsTemplates::k_class_template;

    result = ReplaceAll(result, "__class_scoped_name__", cpp_class.full_name);
    result = ReplaceAll(result, "__class_name__", cpp_class.name);
    result = ReplaceAll(result, "__class_scope__", cpp_class.scope);
    result = ReplaceAll(result, "__class_description__", cpp_class.description);

    // Init properties
    Opal::StringUtf8 properties = "{";
    for (Opal::u64 i = 0; i < cpp_class.properties.GetSize(); i++)
    {
        const CppProperty& prop = cpp_class.properties[i];
        if (i > 0)
        {
            properties += ", ";
        }
        Opal::StringUtf8 is_pod_str = prop.is_pod ? "true" : "false";
        Opal::StringUtf8 offset_expr = "offsetof(" + cpp_class.full_name + ", " + prop.name + ")";
        Opal::StringUtf8 size_expr = "sizeof(std::declval<" + prop.full_type + ">())";
        Opal::StringUtf8 member_type = "decltype(" + cpp_class.full_name + "::" + prop.name + ")";
        Opal::StringUtf8 read_lambda = "[](const void* obj, void* out) { *static_cast<" + member_type
                                        + "*>(out) = static_cast<const " + cpp_class.full_name + "*>(obj)->" + prop.name + "; }";
        Opal::StringUtf8 write_lambda = "[](void* obj, const void* in) { static_cast<" + cpp_class.full_name
                                         + "*>(obj)->" + prop.name + " = *static_cast<const " + member_type + "*>(in); }";
        properties += "{\"" + prop.name + "\", \"" + prop.description + "\", \"" + prop.type + "\", " + is_pod_str
                      + ", " + offset_expr + ", " + size_expr + ", " + read_lambda + ", " + write_lambda + "}";
    }
    properties += "}";
    result = ReplaceAll(result, "__class_init_properties__", properties);

    return result;
}

static Opal::StringUtf8 GenerateEnumCollection(const Opal::DynamicArray<CppEnum>& enums)
{
    Opal::StringUtf8 result = ObsTemplates::k_enum_collection_template;

    Opal::StringUtf8 entries = "{\n";
    for (Opal::u64 i = 0; i < enums.GetSize(); i++)
    {
        const CppEnum& cpp_enum = enums[i];
        if (i > 0)
        {
            entries += ",\n";
        }
        entries += "        {\"" + cpp_enum.name + "\", \"" + cpp_enum.full_name + "\", \"" + cpp_enum.description + "\", "
                   + IntToString(cpp_enum.underlying_type_size) + ", {";

        for (Opal::u64 j = 0; j < cpp_enum.constants.GetSize(); j++)
        {
            const CppEnumConstant& constant = cpp_enum.constants[j];
            if (j > 0)
            {
                entries += ", ";
            }

            Opal::StringUtf8 value_str;
            if (constant.value < 0)
            {
                value_str = "static_cast<uint64_t>(" + IntToString(constant.value) + "LL)";
            }
            else
            {
                value_str = IntToString(constant.value);
            }
            entries += "{\"" + constant.name + "\", \"" + constant.description + "\", " + value_str + "}";
        }
        entries += "}}";
    }
    entries += "\n    }";

    result = ReplaceAll(result, "__enum_collection_entries__", entries);
    return result;
}

static Opal::StringUtf8 GenerateClassCollection(const Opal::DynamicArray<CppClass>& classes)
{
    Opal::StringUtf8 result = ObsTemplates::k_class_collection_template;

    Opal::StringUtf8 entries = "{\n";
    for (Opal::u64 i = 0; i < classes.GetSize(); i++)
    {
        const CppClass& cpp_class = classes[i];
        if (i > 0)
        {
            entries += ",\n";
        }
        entries += "        {\"" + cpp_class.name + "\", \"" + cpp_class.scope + "\", \"" + cpp_class.full_name + "\", \""
                   + cpp_class.description + "\", {";

        for (Opal::u64 j = 0; j < cpp_class.properties.GetSize(); j++)
        {
            const CppProperty& prop = cpp_class.properties[j];
            if (j > 0)
            {
                entries += ", ";
            }
            Opal::StringUtf8 is_pod_str = prop.is_pod ? "true" : "false";
            Opal::StringUtf8 offset_expr = "offsetof(" + cpp_class.full_name + ", " + prop.name + ")";
            Opal::StringUtf8 size_expr = "sizeof(std::declval<" + prop.full_type + ">())";
            Opal::StringUtf8 member_type = "decltype(" + cpp_class.full_name + "::" + prop.name + ")";
            Opal::StringUtf8 read_lambda = "[](const void* obj, void* out) { *static_cast<" + member_type
                                            + "*>(out) = static_cast<const " + cpp_class.full_name + "*>(obj)->" + prop.name + "; }";
            Opal::StringUtf8 write_lambda = "[](void* obj, const void* in) { static_cast<" + cpp_class.full_name
                                             + "*>(obj)->" + prop.name + " = *static_cast<const " + member_type + "*>(in); }";
            entries += "{\"" + prop.name + "\", \"" + prop.description + "\", \"" + prop.type + "\", " + is_pod_str
                       + ", " + offset_expr + ", " + size_expr + ", " + read_lambda + ", " + write_lambda + "}";
        }
        entries += "}}";
    }
    entries += "\n    }";

    result = ReplaceAll(result, "__class_collection_entries__", entries);
    return result;
}

static Opal::StringUtf8 GenerateSingleFile(const CppContext& context)
{
    Opal::StringUtf8 result = ObsTemplates::k_reflection_header_template;

    // Generate includes
    Opal::StringUtf8 includes;
    auto filename = Opal::Paths::GetFileName(context.arguments.input_file);
    if (filename.HasValue())
    {
        includes = "#include \"" + filename.GetValue() + "\"";
    }
    result = ReplaceAll(result, "__refl_includes__", includes);

    // Generate enum specializations
    Opal::StringUtf8 enum_specs;
    for (Opal::u64 i = 0; i < context.enums.GetSize(); i++)
    {
        enum_specs += GenerateEnumSpecialization(context.enums[i]);
        if (i + 1 < context.enums.GetSize())
        {
            enum_specs += "\n";
        }
    }
    result = ReplaceAll(result, "__refl_enum__", enum_specs);

    // Generate class specializations
    Opal::StringUtf8 class_specs;
    for (Opal::u64 i = 0; i < context.classes.GetSize(); i++)
    {
        class_specs += GenerateClassSpecialization(context.classes[i]);
        if (i + 1 < context.classes.GetSize())
        {
            class_specs += "\n";
        }
    }
    result = ReplaceAll(result, "__refl_class__", class_specs);

    // Generate enum collection
    Opal::StringUtf8 enum_collection = GenerateEnumCollection(context.enums);
    result = ReplaceAll(result, "__refl_enum_collection__", enum_collection);

    // Generate class collection
    Opal::StringUtf8 class_collection = GenerateClassCollection(context.classes);
    result = ReplaceAll(result, "__refl_class_collection__", class_collection);

    return result;
}

bool Generate(const CppContext& context)
{
    if (context.arguments.output_dir.IsEmpty())
    {
        printf("Error: Output directory is not set!\n");
        return false;
    }

    if (!Opal::Exists(context.arguments.output_dir))
    {
        Opal::CreateDirectory(context.arguments.output_dir);
    }

    if (!context.arguments.use_separate_files)
    {
        Opal::StringUtf8 content = GenerateSingleFile(context);
        Opal::StringUtf8 output_path = context.arguments.output_dir + "/reflection.hpp";
        if (!WriteToFile(output_path, content))
        {
            printf("Error: Failed to write reflection file: %s\n", output_path.GetData());
            return false;
        }
    }

    // Write obs.h
    Opal::StringUtf8 obs_path = context.arguments.output_dir + "/obs.h";
    if (!WriteToFile(obs_path, Opal::StringUtf8(ObsTemplates::k_obs_header)))
    {
        printf("Error: Failed to write obs.h: %s\n", obs_path.GetData());
        return false;
    }

    return true;
}
