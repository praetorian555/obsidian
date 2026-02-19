#pragma once

#include "opal/types.h"
#include "opal/container/dynamic-array.h"
#include "opal/container/string.h"
#include "opal/exceptions.h"

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

using size_t = u64;

struct CppEnumConstant
{
    Opal::StringUtf8 name;
    Opal::StringUtf8 description;
    Opal::i64 value = -1;
};

struct CppAttribute
{
    Opal::StringUtf8 name;
    Opal::StringUtf8 value;
};

struct CppEnum
{
    Opal::StringUtf8 name;
    Opal::StringUtf8 full_name;
    Opal::StringUtf8 scope;
    Opal::StringUtf8 description;
    Opal::StringUtf8 underlying_type;
    Opal::i64 underlying_type_size = 0;
    bool is_enum_class = false;
    Opal::DynamicArray<CppEnumConstant> constants;
    Opal::DynamicArray<CppAttribute> attributes;
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
};

struct CppClass
{
    Opal::StringUtf8 name;
    Opal::StringUtf8 full_name;
    Opal::StringUtf8 scope;
    Opal::StringUtf8 description;
    bool is_struct = false;
    Opal::i64 alignment = 0;
    Opal::i64 size = 0;
    Opal::DynamicArray<CppProperty> properties;
    Opal::DynamicArray<CppAttribute> attributes;
};

struct ObsidianArguments
{
    Opal::StringUtf8 input_file;
    Opal::StringUtf8 input_dir;
    Opal::StringUtf8 output_dir;
    Opal::DynamicArray<Opal::StringUtf8> compile_options;
    bool should_dump_ast = false;
    bool use_separate_files = false;
    bool verbose = false;
};

struct CppContext
{
    ObsidianArguments arguments;
    Opal::DynamicArray<Opal::StringUtf8> input_files;
    Opal::DynamicArray<CppEnum> enums;
    Opal::DynamicArray<CppClass> classes;
};

struct ArgumentValidationException : Opal::Exception
{
    explicit ArgumentValidationException(const char* message) : Opal::Exception(Opal::StringEx(message)) {}
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
    explicit FileWriteException(const Opal::StringUtf8& file_path)
        : Opal::Exception(Opal::StringEx("Failed to write file: ") + *file_path)
    {
    }
};