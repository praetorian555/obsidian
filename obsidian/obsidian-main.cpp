#include <cstdio>
#include <cstdlib>

#include "opal/container/dynamic-array.h"
#include "opal/container/string.h"
#include "opal/math-base.h"

#include "clang-c/Index.h"

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
    bool is_enum_class = false;
    Opal::DynamicArray<CppEnumConstant> constants;
    Opal::DynamicArray<CppAttribute> attributes;
};

struct CppProperty
{
    Opal::StringUtf8 name;
    Opal::StringUtf8 type;
    Opal::StringUtf8 description;
    Opal::i64 alignment = 0;
    Opal::i64 offset = 0;
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

struct CppContext
{
    Opal::DynamicArray<CppEnum> enums;
    Opal::DynamicArray<CppClass> classes;
};

struct CppTokens
{
    CXToken* data = nullptr;
    Opal::u32 count = 0;
    CXTranslationUnit translation_unit;

    CppTokens(CXTranslationUnit in_tu, CXToken* in_data, Opal::u32 in_count) : data(in_data), count(in_count), translation_unit(in_tu) {}

    ~CppTokens() { clang_disposeTokens(translation_unit, data, count); }
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

bool StartsWith(const Opal::StringUtf8& str, const Opal::StringUtf8& prefix)
{
    if (prefix.GetSize() > str.GetSize())
    {
        return false;
    }
    for (Opal::i32 i = 0; i < prefix.GetSize(); i++)
    {
        if (prefix[i] != str[i])
        {
            return false;
        }
    }
    return true;
}

CppTokens GetPrevTokens(const CXTranslationUnit& translation_unit, const CXCursor& cursor)
{
    CXSourceRange tu_range = clang_getCursorExtent(cursor);
    CXSourceLocation start = clang_getCursorLocation(cursor);
    CXFile file;
    Opal::u32 line, column, offset;
    clang_getSpellingLocation(start, &file, &line, &column, &offset);

    const auto prev_line = static_cast<Opal::u32>(Opal::Max(0, static_cast<Opal::i32>(line) - 1));
    CXSourceLocation new_start = clang_getLocation(translation_unit, file, prev_line, 1);
    CXSourceRange extended_range = clang_getRange(new_start, clang_getRangeEnd(tu_range));

    CXToken* token_data = nullptr;
    Opal::u32 token_count = 0;
    clang_tokenize(translation_unit, extended_range, &token_data, &token_count);
    return {translation_unit, token_data, token_count};
}

void CollectScope(const CXCursor& cursor, Opal::DynamicArray<Opal::StringUtf8>& parents)
{
    CXCursor it = cursor;
    while (true)
    {
        CXCursor parent = clang_getCursorSemanticParent(it);
        if (clang_Cursor_isNull(parent) || clang_getCursorKind(parent) == CXCursor_TranslationUnit)
        {
            break;
        }
        Opal::StringUtf8 parent_name = ToString(clang_getCursorSpelling(parent));
        parents.PushBack(Opal::Move(parent_name));
        it = parent;
    }
}

void CollectAttributes(const Opal::ArrayView<CXToken>& tokens, const CXTranslationUnit& translation_unit,
                       Opal::DynamicArray<CppAttribute>& attributes)
{
    enum class StateMachine
    {
        OpenBracket,
        CollectAttributes,
    };

    StateMachine state = StateMachine::OpenBracket;
    for (Opal::u32 i = 0; i < tokens.GetSize(); i++)
    {
        CXString token_spelling = clang_getTokenSpelling(translation_unit, tokens[i]);
        Opal::StringUtf8 token_name = ToString(token_spelling);

        switch (state)
        {
            case StateMachine::OpenBracket:
            {
                if (token_name != "(")
                {
                    printf("Bad token detected! Expected '('...");
                    return;
                }
                state = StateMachine::CollectAttributes;
                break;
            }
            case StateMachine::CollectAttributes:
            {
                if (token_name == ")")
                {
                    return;
                }
                CXTokenKind token_kind = clang_getTokenKind(tokens[i]);
                if (token_kind == CXToken_Literal)
                {
                    Opal::StringUtf8 parameter = ToString(clang_getTokenSpelling(translation_unit, tokens[i]));
                    if (StartsWith(parameter, "\""))
                    {
                        parameter = Opal::GetSubString(parameter, 1, parameter.GetSize() - 2).GetValue();
                    }
                    CppAttribute attribute;
                    Split(parameter, attribute.name, attribute.value);
                    if (attribute.value.IsEmpty())
                    {
                        attribute.value = "1";
                    }
                    attributes.PushBack(attribute);
                }
                break;
            }
            default:
            {
            }
        }
    }
}

Opal::i32 GetMacroTokenPosition(const CppTokens& tokens, const CXTranslationUnit& translation_unit,
                                const Opal::StringUtf8& macro)
{
    for (Opal::u32 i = 0; i < tokens.count; i++)
    {
        CXString token_spelling = clang_getTokenSpelling(translation_unit, tokens.data[i]);
        Opal::StringUtf8 token_name = ToString(token_spelling);
        if (token_name == macro)
        {
            return i;
        }
    }
    return -1;
}

Opal::StringUtf8 GetEnumConstantDescription(CXCursor cursor)
{
    CXSourceRange comment_range = clang_Cursor_getCommentRange(cursor);
    CXSourceLocation comment_range_end = clang_getRangeEnd(comment_range);
    CXSourceLocation cursor_start = clang_getCursorLocation(cursor);
    Opal::u32 comment_end_line;
    Opal::u32 cursor_start_line;
    clang_getSpellingLocation(comment_range_end, nullptr, &comment_end_line, nullptr, nullptr);
    clang_getSpellingLocation(cursor_start, nullptr, &cursor_start_line, nullptr, nullptr);
    if (cursor_start_line - comment_end_line > 1)
    {
        return {};
    }
    return ToString(clang_Cursor_getBriefCommentText(cursor));
}

CXChildVisitResult VisitorEnumConstant(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
    CXCursorKind kind = clang_getCursorKind(cursor);
    if (kind != CXCursor_EnumConstantDecl)
    {
        return CXChildVisit_Continue;
    }

    auto* cpp_enum = static_cast<CppEnum*>(client_data);
    CppEnumConstant enum_constant;
    enum_constant.name = ToString(clang_getCursorSpelling(cursor));
    enum_constant.description = GetEnumConstantDescription(cursor);
    enum_constant.value = clang_getEnumConstantDeclValue(cursor);
    cpp_enum->constants.PushBack(Opal::Move(enum_constant));

    return CXChildVisit_Continue;
}

void VisitEnum(CXCursor cursor, CppContext& context)
{
    CXString name_spelling = clang_getCursorSpelling(cursor);
    Opal::StringUtf8 name = ToString(name_spelling);

    CXTranslationUnit translation_unit = clang_Cursor_getTranslationUnit(cursor);
    CppTokens tokens = GetPrevTokens(translation_unit, cursor);
    Opal::i32 qualifier_pos = GetMacroTokenPosition(tokens, translation_unit, "OBS_ENUM");
    if (qualifier_pos >= 0)
    {
        CppEnum cpp_enum{.name = name, .description = ToString(clang_Cursor_getBriefCommentText(cursor))};
        CXType underlying_type = clang_getEnumDeclIntegerType(cursor);
        cpp_enum.underlying_type = ToString(clang_getTypeSpelling(underlying_type));
        cpp_enum.is_enum_class = clang_EnumDecl_isScoped(cursor) != 0;
        CollectAttributes({tokens.data + 1, tokens.count - 1}, translation_unit, cpp_enum.attributes);
        clang_visitChildren(cursor, VisitorEnumConstant, &cpp_enum);

        Opal::DynamicArray<Opal::StringUtf8> parents;
        CollectScope(cursor, parents);
        Opal::StringUtf8 scope;
        for (Opal::i32 i = static_cast<Opal::i32>(parents.GetSize()) - 1; i >= 0; i--)
        {
            scope += parents[i] + "::";
        }
        if (!scope.IsEmpty())
        {
            scope = Opal::GetSubString(scope, 0, scope.GetSize() - 2).GetValue();
        }
        cpp_enum.full_name = scope + "::" + cpp_enum.name;
        cpp_enum.scope = Opal::Move(scope);

        context.enums.PushBack(cpp_enum);
    }
}

CXChildVisitResult VisitorClassProperty(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
    CXCursorKind kind = clang_getCursorKind(cursor);
    if (kind != CXCursor_FieldDecl)
    {
        return CXChildVisit_Continue;
    }

    CXTranslationUnit translation_unit = clang_Cursor_getTranslationUnit(cursor);
    CppTokens tokens = GetPrevTokens(translation_unit, cursor);
    Opal::i32 qualifier_pos = GetMacroTokenPosition(tokens, translation_unit, "OBS_PROP");
    if (qualifier_pos < 0)
    {
        return CXChildVisit_Continue;
    }

    CppProperty property;
    property.name = ToString(clang_getCursorSpelling(cursor));
    CXType type = clang_getCursorType(cursor);
    property.type = ToString(clang_getTypeSpelling(type));
    property.description = GetEnumConstantDescription(cursor);
    property.alignment = clang_Type_getAlignOf(type);
    property.offset = clang_Cursor_getOffsetOfField(cursor) / 8;
    CollectAttributes({tokens.data + 1, tokens.count - 1}, translation_unit, property.attributes);

    auto* cpp_class = static_cast<CppClass*>(client_data);
    cpp_class->properties.PushBack(Opal::Move(property));

    return CXChildVisit_Continue;
}

void VisitClass(CXCursor cursor, CppContext& context)
{
    CXString name_spelling = clang_getCursorSpelling(cursor);
    Opal::StringUtf8 name = ToString(name_spelling);

    CXTranslationUnit translation_unit = clang_Cursor_getTranslationUnit(cursor);
    CppTokens tokens = GetPrevTokens(translation_unit, cursor);
    Opal::i32 qualifier_pos = GetMacroTokenPosition(tokens, translation_unit, "OBS_CLASS");
    if (qualifier_pos >= 0)
    {
        CppClass cpp_class{.name = name, .description = ToString(clang_Cursor_getBriefCommentText(cursor))};
        cpp_class.is_struct = clang_getCursorKind(cursor) == CXCursor_StructDecl;
        CXType type = clang_getCursorType(cursor);
        cpp_class.alignment = clang_Type_getAlignOf(type);
        cpp_class.size = clang_Type_getSizeOf(type);
        CollectAttributes({tokens.data + 1, tokens.count - 1}, translation_unit, cpp_class.attributes);

        clang_visitChildren(cursor, VisitorClassProperty, &cpp_class);

        Opal::DynamicArray<Opal::StringUtf8> parents;
        CollectScope(cursor, parents);
        Opal::StringUtf8 scope;
        for (Opal::i32 i = static_cast<Opal::i32>(parents.GetSize()) - 1; i >= 0; i--)
        {
            scope += parents[i] + "::";
        }
        if (!scope.IsEmpty())
        {
            scope = Opal::GetSubString(scope, 0, scope.GetSize() - 2).GetValue();
        }
        cpp_class.full_name = scope + "::" + cpp_class.name;
        cpp_class.scope = Opal::Move(scope);

        context.classes.PushBack(cpp_class);
    }
}

CXChildVisitResult Visitor(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
    CXCursorKind kind = clang_getCursorKind(cursor);
    auto* context = static_cast<CppContext*>(client_data);

    if (kind == CXCursor_EnumDecl)
    {
        VisitEnum(cursor, *context);
    }
    else if (kind == CXCursor_ClassDecl || kind == CXCursor_StructDecl)
    {
        VisitClass(cursor, *context);
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

    CppContext context;
    clang_visitChildren(cursor, Visitor, &context);

    printf("Enums:\n");
    for (Opal::i32 i = 0; i < context.enums.GetSize(); i++)
    {
        printf("\t%s:\n", context.enums[i].name.GetData());
        printf("\t\tFull Name: %s\n", context.enums[i].full_name.GetData());
        printf("\t\tScope: %s\n", context.enums[i].scope.GetData());
        printf("\t\tUnderlying Type: %s\n", context.enums[i].underlying_type.GetData());
        printf("\t\tIs Enum Class: %s\n", context.enums[i].is_enum_class ? "Yes" : "No");
        printf("\t\tDescription: %s\n", context.enums[i].description.GetData());
        printf("\t\tConstants:\n");
        for (Opal::u32 j = 0; j < context.enums[i].constants.GetSize(); j++)
        {
            const CppEnumConstant& constant = context.enums[i].constants[j];
            printf("\t\t\tname=%s, value=%lld, description=%s\n", constant.name.GetData(), constant.value, constant.description.GetData());
        }
        printf("\t\tAttributes:\n");
        for (Opal::u32 j = 0; j < context.enums[i].attributes.GetSize(); j++)
        {
            const CppAttribute& attr = context.enums[i].attributes[j];
            printf("\t\t\t%s=%s\n", attr.name.GetData(), attr.value.GetData());
        }
    }

    printf("Classes:\n");
    for (Opal::i32 i = 0; i < context.classes.GetSize(); i++)
    {
        printf("\t%s:\n", context.classes[i].name.GetData());
        printf("\t\tFull Name: %s\n", context.classes[i].full_name.GetData());
        printf("\t\tScope: %s\n", context.classes[i].scope.GetData());
        printf("\t\tIs Struct: %s\n", context.classes[i].is_struct ? "Yes" : "No");
        printf("\t\tDescription: %s\n", context.classes[i].description.GetData());
        printf("\t\tAlignment: %lld\n", context.classes[i].alignment);
        printf("\t\tSize: %lld\n", context.classes[i].size);
        printf("\t\tProperties:\n");
        for (Opal::u32 j = 0; j < context.classes[i].properties.GetSize(); j++)
        {
            const CppProperty& property = context.classes[i].properties[j];
            printf("\t\t\tname=%s, type=%s, offset=%lld, alignment=%lld, description=%s\n", property.name.GetData(),
                   property.type.GetData(), property.offset, property.alignment, property.description.GetData());
        }
        printf("\t\tAttributes:\n");
        for (Opal::u32 j = 0; j < context.classes[i].attributes.GetSize(); j++)
        {
            const CppAttribute& attr = context.classes[i].attributes[j];
            printf("\t\t\t%s=%s\n", attr.name.GetData(), attr.value.GetData());
        }
    }

    clang_disposeTranslationUnit(translation_unit);
    clang_disposeIndex(index);
    return 0;
}
