#pragma once

#include "opal/container/dynamic-array.h"
#include "opal/container/string.h"
#include "opal/exceptions.h"
#include "opal/logging.h"
#include "opal/types.h"

using i8 = Opal::i8;
using i16 = Opal::i16;
using i32 = Opal::i32;
using i64 = Opal::i64;

using u8 = Opal::u8;
using u16 = Opal::u16;
using u32 = Opal::u32;
using u64 = Opal::u64;

using f32 = Opal::f32;
using f64 = Opal::f64;

struct CppEnumConstant
{
    Opal::StringUtf8 name;
    Opal::StringUtf8 description;
    Opal::i64 value = -1;

    CppEnumConstant Clone(Opal::AllocatorBase* = nullptr) const
    {
        CppEnumConstant clone;
        clone.name = name.Clone();
        clone.description = description.Clone();
        clone.value = value;
        return clone;
    }
};

struct CppAttribute
{
    Opal::StringUtf8 name;
    Opal::StringUtf8 value;

    CppAttribute Clone(Opal::AllocatorBase* = nullptr) const
    {
        CppAttribute clone;
        clone.name = name.Clone();
        clone.value = value.Clone();
        return clone;
    }
};

struct CppEnum
{
    Opal::StringUtf8 containing_file_path;
    Opal::StringUtf8 name;
    Opal::StringUtf8 full_name;
    Opal::StringUtf8 scope;
    Opal::StringUtf8 description;
    Opal::StringUtf8 underlying_type;
    Opal::i64 underlying_type_size = 0;
    bool is_enum_class = false;
    Opal::DynamicArray<CppEnumConstant> constants;
    Opal::DynamicArray<CppAttribute> attributes;

    CppEnum Clone(Opal::AllocatorBase* = nullptr) const
    {
        CppEnum clone;
        clone.containing_file_path = containing_file_path.Clone();
        clone.name = name.Clone();
        clone.full_name = full_name.Clone();
        clone.scope = scope.Clone();
        clone.description = description.Clone();
        clone.underlying_type = underlying_type.Clone();
        clone.underlying_type_size = underlying_type_size;
        clone.is_enum_class = is_enum_class;
        clone.constants = constants.Clone();
        clone.attributes = attributes.Clone();
        return clone;
    }
};

struct CppProperty
{
    Opal::StringUtf8 name;
    Opal::StringUtf8 type;
    Opal::StringUtf8 type_scope;
    Opal::StringUtf8 full_type;
    Opal::StringUtf8 description;
    bool is_pod = false;
    Opal::i64 alignment = 0;
    Opal::i64 offset = 0;
    Opal::i64 size = 0;
    Opal::DynamicArray<CppAttribute> attributes;

    CppProperty Clone(Opal::AllocatorBase* = nullptr) const
    {
        CppProperty clone;
        clone.name = name.Clone();
        clone.type = type.Clone();
        clone.type_scope = type_scope.Clone();
        clone.full_type = full_type.Clone();
        clone.description = description.Clone();
        clone.is_pod = is_pod;
        clone.alignment = alignment;
        clone.offset = offset;
        clone.size = size;
        clone.attributes = attributes.Clone();
        return clone;
    }
};

struct CppClass
{
    Opal::StringUtf8 containing_file_path;
    Opal::StringUtf8 name;
    Opal::StringUtf8 full_name;
    Opal::StringUtf8 scope;
    Opal::StringUtf8 description;
    bool is_struct = false;
    Opal::i64 alignment = 0;
    Opal::i64 size = 0;
    Opal::DynamicArray<CppProperty> properties;
    Opal::DynamicArray<CppAttribute> attributes;

    CppClass Clone(Opal::AllocatorBase* = nullptr) const
    {
        CppClass clone;
        clone.containing_file_path = containing_file_path.Clone();
        clone.name = name.Clone();
        clone.full_name = full_name.Clone();
        clone.scope = scope.Clone();
        clone.description = description.Clone();
        clone.is_struct = is_struct;
        clone.alignment = alignment;
        clone.size = size;
        clone.properties = properties.Clone();
        clone.attributes = attributes.Clone();
        return clone;
    }
};

struct ObsidianArguments
{
    Opal::DynamicArray<Opal::StringUtf8> input_files;
    Opal::DynamicArray<Opal::StringUtf8> input_dirs;
    Opal::StringUtf8 output_dir;
    Opal::StringUtf8 standard_version = "-std=c++20";
    Opal::DynamicArray<Opal::StringUtf8> compile_options;
    Opal::DynamicArray<Opal::StringUtf8> include_directories;
    bool should_dump_ast = false;
    bool use_separate_files = false;
    Opal::LogLevel log_level = Opal::LogLevel::Error;

    Opal::DynamicArray<Opal::StringUtf8> include_directories_as_option;
};

struct CppContext
{
    ObsidianArguments arguments;
    Opal::DynamicArray<Opal::StringUtf8> input_files;
    Opal::DynamicArray<CppEnum> enums;
    Opal::DynamicArray<CppClass> classes;
    Opal::DynamicArray<Opal::StringUtf8> files_to_include;

    f32 cache_duration = 0.0f;
    f32 compilation_duration = 0.0f;
    f32 generation_duration = 0.0f;
};

struct ArgumentValidationException : Opal::Exception
{
    explicit ArgumentValidationException(const Opal::StringUtf8& message) : Opal::Exception(Opal::StringEx(*message)) {}
};

struct TranslationFailedException : Opal::Exception
{
    explicit TranslationFailedException(const Opal::StringUtf8& file_path)
        : Opal::Exception(Opal::StringEx("Failed to compile C++ file: ") + *file_path)
    {
    }
};

struct FileWriteException : Opal::Exception
{
    explicit FileWriteException(const Opal::StringUtf8& file_path) : Opal::Exception(Opal::StringEx("Failed to write file: ") + *file_path)
    {
    }
};