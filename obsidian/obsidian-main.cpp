#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "opal/container/dynamic-array.h"
#include "opal/container/string.h"
#include "opal/math-base.h"

#include "clang-c/Index.h"

struct CppEnumValue
{
    Opal::StringUtf8 name;
    Opal::StringUtf8 value;
};

struct CppAttribute
{
    Opal::StringUtf8 name;
    Opal::StringUtf8 value;
};

struct CppEnum
{
    Opal::StringUtf8 name;
    Opal::DynamicArray<CppEnumValue> values;
    Opal::DynamicArray<CppAttribute> attributes;
};

Opal::StringUtf8 ToString(const CXString& clang_str)
{
    Opal::StringUtf8 str = clang_getCString(clang_str);
    clang_disposeString(clang_str);
    return str;
}

bool Split(const Opal::StringUtf8& in, Opal::StringUtf8& name, Opal::StringUtf8& value)
{
    Opal::StringUtf8::size_type pos = Opal::Find(in, "=");
    if (pos == Opal::StringUtf8::k_npos)
    {
        name = in;
        return false;
    }
    name = Opal::GetSubString(in, 0, pos).GetValue();
    value = Opal::GetSubString(in, pos + 1, Opal::StringUtf8::k_npos).GetValue();
    return true;
}

CXChildVisitResult Visitor(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
    CXCursorKind kind = clang_getCursorKind(cursor);

    if (kind == CXCursor_EnumDecl)
    {
        CXString name_spelling = clang_getCursorSpelling(cursor);
        Opal::StringUtf8 name = ToString(name_spelling);

        CXTranslationUnit translation_unit = clang_Cursor_getTranslationUnit(cursor);
        CXSourceRange tu_range = clang_getCursorExtent(cursor);
        CXSourceLocation start = clang_getCursorLocation(cursor);
        CXFile file;
        Opal::u32 line, column, offset;
        clang_getSpellingLocation(start, &file, &line, &column, &offset);

        constexpr Opal::i32 k_go_back_bytes = 200;

        const Opal::u32 prev_line = static_cast<Opal::u32>(Opal::Max(0, static_cast<Opal::i32>(line) - 1));
        CXSourceLocation new_start = clang_getLocation(translation_unit, file, prev_line, 1);
        CXSourceRange extended_range = clang_getRange(new_start, clang_getRangeEnd(tu_range));

        CXToken* tokens = nullptr;
        Opal::u32 tokens_count = 0;
        clang_tokenize(translation_unit, extended_range, &tokens, &tokens_count);

        for (Opal::u32 i = 0; i < tokens_count; i++)
        {
            CXString token_spelling = clang_getTokenSpelling(translation_unit, tokens[i]);
            Opal::StringUtf8 token_name = ToString(token_spelling);
            if (token_name == "OBS_ENUM")
            {
                CppEnum enum_obj;
                enum_obj.name = name;
                printf("Enum: %s\n", name.GetData());
                i++;
                if (i < tokens_count)
                {
                    token_name = ToString(clang_getTokenSpelling(translation_unit, tokens[i]));
                    if (token_name == "(")
                    {
                        i++;
                        while (i < tokens_count)
                        {
                            token_name = ToString(clang_getTokenSpelling(translation_unit, tokens[i]));
                            if (token_name == ")")
                            {
                                break;
                            }
                            CXTokenKind token_kind = clang_getTokenKind(tokens[i]);
                            if (token_kind == CXToken_Literal)
                            {
                                Opal::StringUtf8 parameter = ToString(clang_getTokenSpelling(translation_unit, tokens[i]));
                                parameter = Opal::GetSubString(parameter, 1, parameter.GetSize() - 2).GetValue();
                                CppAttribute attribute;
                                Split(parameter, attribute.name, attribute.value);
                                printf("\tAttribute: name=%s, value=%s\n", attribute.name.GetData(), attribute.value.GetData());
                                enum_obj.attributes.PushBack(attribute);
                            }
                            i++;
                        }
                    }
                }
                break;
            }
        }
    }

    return CXChildVisit_Recurse;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    CXIndex index = clang_createIndex(0, 0);

    const char* args[] = {"-Wall", "-std=c++20", "-DDONT_CRASH=1"};
    CXTranslationUnit translation_unit =
        clang_parseTranslationUnit(index, argv[1], args, 3, nullptr, 0, CXTranslationUnit_DetailedPreprocessingRecord);
    if (translation_unit == nullptr)
    {
        printf("Failed to parse translation_unit\n");
        return 1;
    }

    // Check for diagnostics
    unsigned numDiags = clang_getNumDiagnostics(translation_unit);
    for (unsigned i = 0; i < numDiags; i++)
    {
        CXDiagnostic diag = clang_getDiagnostic(translation_unit, i);
        CXString diagStr = clang_formatDiagnostic(diag, CXDiagnostic_DisplaySourceLocation);
        printf("Diagnostic: %s\n", clang_getCString(diagStr));
        clang_disposeString(diagStr);
        clang_disposeDiagnostic(diag);
    }

    CXCursor cursor = clang_getTranslationUnitCursor(translation_unit);

    clang_visitChildren(cursor, Visitor, nullptr);

    clang_disposeTranslationUnit(translation_unit);
    clang_disposeIndex(index);
    return 0;
}
