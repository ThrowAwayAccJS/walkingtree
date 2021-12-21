#pragma once

#include <vector>
#include <optional>
#include <string>
#include <unordered_map>
#include "Tokenizer.h"
#include "Type.h"
#include "FunctionDefinition.h"

namespace parser
{
    using namespace std;

    class Parser
    {
    public:
        Parser();
        std::unordered_map<std::string, FunctionDefinition> parse(vector<Token> &tokens);
        void debug_print() const;
    private:
        std::vector<Token>::iterator end_token;
        std::vector<Token>::iterator current_token;
        std::unordered_map<std::string, Type> types;
        std::unordered_map<std::string, FunctionDefinition> functions;

        //typedef optional<Statement> (*PreBuiltStatement)(void);

        std::optional<Statement> for_statement();
        std::optional<Statement> if_statement();
        std::optional<Statement> else_statement();

        //map<string, PreBuiltStatement> pre_built_statements;

        std::optional<Token> expect_identifer(const string &name = "");
        std::optional<Token> expect_operator(const string &name = "");
        std::optional<Statement> expect_statement();
        std::optional<Statement> expect_expression();
        std::optional<Statement> expect_variable_declaration();
        std::optional<Statement> expect_assignment();
        std::optional<Statement> expect_one_value();
        std::optional<Statement> expect_function_call();
        std::optional<Statement> array_literal_expression();
        std::optional<Statement> expect_property_access(Statement &other);
        std::optional<Statement> expect_index_access(const bool with_value = false);
        std::optional<Statement> expect_array_definition(Statement statement);
        std::optional<vector<Statement>> parse_function_body();
        Statement* find_rightmost_statement(Statement *lhs, size_t rhs_precedence);
        size_t operator_precedence(const std::string& operator_name);
        bool expect_function_definition();
        bool expect_external_function_definition();
        std::optional<Type> expect_type();
    };
}