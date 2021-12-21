#pragma once

#include <string>
#include "Type.h"

namespace parser
{
    enum StatementKind
    {
        VARIABLE_DECLARATION,
        ASSIGNMENT_EXPRESSION,
        ASSIGNMENT_PLUS_EXPRESSION,
        ASSIGNMENT_MINUS_EXPRESSION,
        ASSIGNMENT_MULTIPLY_EXPRESSION,
        ASSIGNMENT_DIVIDE_EXPRESSION,
        PROPETY_ACCESS_EXPRESSION,
        ELEMENT_ACCESS_EXPRESSION,
        INDEX,
        FUNCTION_CALL,
        LITERAL,
        VECTOR_LITERAL,
        OPERATOR_CALL,
        FOR_STATEMENT,
        VARIABLE,
        BLOCK_STATEMENT,
        BODY,
        RETURN_STATEMENT,
        IF_STATEMENT,
    };

    static const char* statement_kind_strings[] =
    {
        "VARIABLE_DECLARATION",
        "ASSIGNMENT_EXPRESSION",
        "ASSIGNMENT_PLUS_EXPRESSION",
        "ASSIGNMENT_MINUS_EXPRESSION",
        "ASSIGNMENT_MULTIPLY_EXPRESSION",
        "ASSIGNMENT_DIVIDE_EXPRESSION",
        "PROPETY_ACCESS_EXPRESSION",
        "ELEMENT_ACCESS_EXPRESSION",
        "INDEX",
        "FUNCTION_CALL",
        "LITERAL",
        "VECTOR_LITERAL",
        "OPERATOR_CALL",
        "FOR_STATEMENT",
        "VARIABLE",
        "BLOCK_STATEMENT",
        "BODY",
        "RETURN_STATEMENT",
        "IF_STATEMENT",
    };

    class Statement
    {
    public:
        std::string name;
        Type type {"void", TYPE::VOID};
        std::vector<Statement> parameters;
        StatementKind kind{StatementKind::FUNCTION_CALL};
        void debug_print(size_t indent);
    };
}