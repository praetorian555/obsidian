#include <cstdio>

#include "opal/container/string.h"

#include <clang-c/Documentation.h>
#include <clang-c/Index.h>

CXChildVisitResult visitor(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
    CXCursorKind kind = clang_getCursorKind(cursor);

    if (kind == CXCursor_ClassDecl || kind == CXCursor_StructDecl)
    {
        CXString name = clang_getCursorSpelling(cursor);
        Opal::StringUtf8 str = clang_getCString(name);
        if (str.IsEmpty() || str[0] == '_')
        {
            return CXChildVisit_Recurse;
        }

        CXString type = clang_getCursorKindSpelling(kind);
        printf("%s", clang_getCString(type));
        clang_disposeString(type);

        printf(" %s\n", clang_getCString(name));
        clang_disposeString(name);

        CXString comment = clang_Cursor_getBriefCommentText(cursor);
        printf("Description: %s\n", clang_getCString(comment));
        clang_disposeString(comment);
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

    CXTranslationUnit translation_unit = clang_parseTranslationUnit(index, argv[1], nullptr, 0, nullptr, 0, CXTranslationUnit_None);
    if (translation_unit == nullptr)
    {
        printf("Failed to parse translation_unit\n");
        return 1;
    }

    CXCursor cursor = clang_getTranslationUnitCursor(translation_unit);

    clang_visitChildren(cursor, visitor, nullptr);

    clang_disposeTranslationUnit(translation_unit);
    clang_disposeIndex(index);
    return 0;
}
