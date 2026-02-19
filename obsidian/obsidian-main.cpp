#include <cstdio>

#include "opal/file-system.h"
#include "opal/logging.h"
#include "opal/math-base.h"
#include "opal/paths.h"
#include "opal/program-arguments.h"

#include "clang-c/Index.h"

#include "generator.hpp"
#include "templates.hpp"
#include "types.hpp"

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
                    Opal::GetLogger().Error("Obsidian", "Bad token detected! Expected '('...");
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
                    if (StartsWith<Opal::StringUtf8>(parameter, "\""))
                    {
                        parameter = Opal::GetSubString(parameter, 1, parameter.GetSize() - 2).GetValue();
                    }
                    if (parameter.IsEmpty())
                    {
                        Opal::GetLogger().Warning("Obsidian", "Skipping empty attribute");
                        break;
                    }
                    CppAttribute attribute;
                    Opal::Split<Opal::StringUtf8>(parameter, "=", attribute.name, attribute.value);
                    if (attribute.name.IsEmpty())
                    {
                        Opal::GetLogger().Warning("Obsidian", "Skipping malformed attribute: {}", parameter.GetData());
                        break;
                    }
                    if (!attribute.value.IsEmpty() && Opal::Find(attribute.value, '=') != Opal::StringUtf8::k_npos)
                    {
                        Opal::GetLogger().Warning("Obsidian", "Skipping malformed attribute with multiple '=' signs: {}",
                                                  parameter.GetData());
                        break;
                    }
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

Opal::i32 GetMacroTokenPosition(const CppTokens& tokens, const CXTranslationUnit& translation_unit, const Opal::StringUtf8& macro)
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
    Opal::GetLogger().Verbose("Obsidian", "  Detected enum constant: {} = {}", enum_constant.name.GetData(),
                              enum_constant.value);
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
        cpp_enum.underlying_type_size = clang_Type_getSizeOf(underlying_type);
        cpp_enum.is_enum_class = clang_EnumDecl_isScoped(cursor) != 0;
        CollectAttributes({tokens.data + 1, tokens.count - 1}, translation_unit, cpp_enum.attributes);
        Opal::GetLogger().Verbose("Obsidian", "Detected enum: {} (attributes: {})", name.GetData(),
                                  cpp_enum.attributes.GetSize());
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

    CXCursor type_decl = clang_getTypeDeclaration(type);
    if (!clang_Cursor_isNull(type_decl))
    {
        Opal::DynamicArray<Opal::StringUtf8> parents;
        CollectScope(type_decl, parents);
        Opal::StringUtf8 scope;
        for (Opal::i32 i = static_cast<Opal::i32>(parents.GetSize()) - 1; i >= 0; i--)
        {
            scope += parents[i] + "::";
        }
        if (!scope.IsEmpty())
        {
            scope = Opal::GetSubString(scope, 0, scope.GetSize() - 2).GetValue();
        }
        property.type_scope = scope;
        property.full_type = scope.IsEmpty() ? property.type : scope + "::" + property.type;
    }
    else
    {
        property.full_type = property.type;
    }

    property.description = ToString(clang_Cursor_getBriefCommentText(cursor));
    property.is_pod = clang_isPODType(type) != 0;
    property.alignment = clang_Type_getAlignOf(type);
    property.offset = clang_Cursor_getOffsetOfField(cursor) / 8;
    property.size = clang_Type_getSizeOf(type);
    CollectAttributes({tokens.data + 1, tokens.count - 1}, translation_unit, property.attributes);
    Opal::GetLogger().Verbose("Obsidian", "  Detected property: {} (type: {}, attributes: {})", property.name.GetData(),
                              property.type.GetData(), property.attributes.GetSize());

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
        Opal::GetLogger().Verbose("Obsidian", "Detected class: {} (attributes: {})", name.GetData(),
                                  cpp_class.attributes.GetSize());

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

void DumpAst(const CppContext& context)
{
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
}

CXTranslationUnit ParseTranslationUnit(const Opal::StringUtf8& input_file, CXIndex index, Opal::ArrayView<Opal::StringUtf8> args)
{
    Opal::DynamicArray<const char*> args_array(args.GetSize());
    Opal::StringUtf8 options_str;
    for (Opal::u32 i = 0; i < args.GetSize(); i++)
    {
        args_array[i] = args[i].GetData();
        if (i > 0)
        {
            options_str += " ";
        }
        options_str += args[i];
    }
    Opal::GetLogger().Verbose("Obsidian", "Compile options: {}", options_str.GetData());
    CXTranslationUnit translation_unit = clang_parseTranslationUnit(index, input_file.GetData(), args_array.GetData(), args_array.GetSize(),
                                                                    nullptr, 0, CXTranslationUnit_DetailedPreprocessingRecord);

    if (translation_unit == nullptr)
    {
        throw TranslationFailedException(input_file);
    }

    bool has_errors = false;
    Opal::u32 num_diagnostics = clang_getNumDiagnostics(translation_unit);
    for (Opal::u32 i = 0; i < num_diagnostics; i++)
    {
        CXDiagnostic diagnostic = clang_getDiagnostic(translation_unit, i);
        CXDiagnosticSeverity severity = clang_getDiagnosticSeverity(diagnostic);
        Opal::StringUtf8 message = ToString(clang_formatDiagnostic(diagnostic, clang_defaultDiagnosticDisplayOptions()));
        clang_disposeDiagnostic(diagnostic);

        if (severity >= CXDiagnostic_Error)
        {
            Opal::GetLogger().Error("Obsidian", "{}", message.GetData());
            has_errors = true;
        }
        else if (severity == CXDiagnostic_Warning)
        {
            Opal::GetLogger().Warning("Obsidian", "{}", message.GetData());
        }
        else if (severity == CXDiagnostic_Note)
        {
            Opal::GetLogger().Verbose("Obsidian", "{}", message.GetData());
        }
    }

    if (has_errors)
    {
        clang_disposeTranslationUnit(translation_unit);
        throw TranslationFailedException(input_file);
    }

    return translation_unit;
}

void ProcessTranslationUnit(CppContext& context, CXIndex index, const Opal::StringUtf8& input_file)
{
    auto compile_options = Opal::ArrayView<Opal::StringUtf8>{context.arguments.compile_options};
    CXTranslationUnit translation_unit = ParseTranslationUnit(input_file, index, compile_options);
    CXCursor cursor = clang_getTranslationUnitCursor(translation_unit);
    clang_visitChildren(cursor, Visitor, &context);
    clang_disposeTranslationUnit(translation_unit);
}

void Run(ObsidianArguments& arguments)
{
    CXIndex index = clang_createIndex(0, 0);

    CppContext context{.arguments = arguments};
    if (!arguments.input_file.IsEmpty())
    {
        Opal::GetLogger().Info("Obsidian", "Processing file: {}", arguments.input_file.GetData());
        ProcessTranslationUnit(context, index, arguments.input_file);
        Opal::GetLogger().Info("Obsidian", "Successfully compiled: {}", arguments.input_file.GetData());
        context.input_files.PushBack(arguments.input_file);
    }
    else if (!arguments.input_dir.IsEmpty())
    {
        Opal::DynamicArray<Opal::DirectoryEntry> entries =
            Opal::CollectDirectoryContents(arguments.input_dir, {.include_directories = false, .recursive = true});
        for (Opal::u64 i = 0; i < entries.GetSize(); i++)
        {
            auto ext = Opal::Paths::GetExtension(entries[i].path);
            if (!ext.HasValue())
            {
                continue;
            }
            if (ext.GetValue() != ".h" && ext.GetValue() != ".hpp")
            {
                continue;
            }
            Opal::GetLogger().Info("Obsidian", "Processing file: {}", entries[i].path.GetData());
            ProcessTranslationUnit(context, index, entries[i].path);
            Opal::GetLogger().Info("Obsidian", "Successfully compiled: {}", entries[i].path.GetData());
            context.input_files.PushBack(entries[i].path);
        }
    }

    Opal::GetLogger().Info("Obsidian", "Found {} enums and {} classes", context.enums.GetSize(), context.classes.GetSize());

    if (context.arguments.should_dump_ast)
    {
        DumpAst(context);
    }

    if (!context.arguments.output_dir.IsEmpty())
    {
        Opal::GetLogger().Info("Obsidian", "Generating reflection data...");
        Generate(context);
        Opal::GetLogger().Info("Obsidian", "Reflection data generated successfully");
    }

    clang_disposeIndex(index);
}

ObsidianArguments ParseAndValidateArguments(int argc, const char** argv)
{
    ObsidianArguments arguments;
    Opal::ProgramArgumentsBuilder builder;
    builder.AddProgramDescription("Obsidian - C++ reflection code generation tool")
        .AddUsageExample("obsidian input-file=my_types.hpp output-dir=generated compile-options=\"-I/usr/include,-DMY_DEFINE\"")
        .AddArgumentDefinition(arguments.input_file, {"input-file", "Path to a single input header file", true})
        .AddArgumentDefinition(arguments.input_dir, {"input-dir", "Path to a directory with input header files", true})
        .AddArgumentDefinition(arguments.output_dir, {"output-dir", "Path to the output directory for generated headers", true})
        .AddArgumentDefinition(arguments.should_dump_ast, {"dump-ast", "Dump the extracted AST metadata", true})
        .AddArgumentDefinition(arguments.use_separate_files, {"separate-files", "Generate separate files for each type", true})
        .AddArgumentDefinition(arguments.compile_options, {"compile-options", "Comma-separated list of compile options", true})
        .AddArgumentDefinition(arguments.verbose, {"verbose", "Enable verbose logging output", true});

    if (!builder.Build(argv, static_cast<Opal::u32>(argc)))
    {
        throw ArgumentValidationException("Failed to build program arguments");
    }

    if (!arguments.input_file.IsEmpty() && !arguments.input_dir.IsEmpty())
    {
        throw ArgumentValidationException("You must specify either input-file or input-dir, not both");
    }
    if (!arguments.input_file.IsEmpty() && !Opal::Exists(arguments.input_file))
    {
        throw ArgumentValidationException("Input file does not exist");
    }
    if (!arguments.input_dir.IsEmpty() && !Opal::Exists(arguments.input_dir))
    {
        throw ArgumentValidationException("Input directory does not exist");
    }
    if (!arguments.output_dir.IsEmpty() && !Opal::Exists(arguments.output_dir))
    {
        throw ArgumentValidationException("Output directory does not exist");
    }

    return arguments;
}

int main(int argc, const char** argv)
{
    Opal::Logger logger;
    logger.SetPattern("<level>: <message>\n");
    auto sink = Opal::MakeShared<Opal::LogSink, Opal::ConsoleSink>(nullptr);
    logger.AddSink(sink);
    logger.RegisterCategory("Obsidian", Opal::LogLevel::Info);
    Opal::SetLogger(&logger);

    try
    {
        ObsidianArguments arguments = ParseAndValidateArguments(argc, argv);
        if (arguments.verbose)
        {
            logger.SetCategoryLevel("Obsidian", Opal::LogLevel::Verbose);
        }
        Run(arguments);
    }
    catch (const Opal::Exception& e)
    {
        Opal::GetLogger().Error("Obsidian", "{}", *e.What());
        Opal::GetLogger().Flush();
        return 1;
    }
    return 0;
}
