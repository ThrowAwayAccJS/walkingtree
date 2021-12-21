#pragma once

#include <vector>

namespace parser
{
    using namespace std;

    enum TokenType
    {
        STRING_LITERAL,
        INTEGER_LITERAL,
        DOUBLE_LITERAL,
        IDENTIFIER,
        OPERATOR,
        WHITESPACE,
        POTENTIAL_DOUBLE,
        POTENTIAL_COMMENT,
        COMMENT,
        BINARY_EXPRESSION,
    };

    static const char* token_type_strings[] = 
    {
        "STRING_LITERAL",
        "INTEGER_LITERAL",
        "DOUBLE_LITERAL",
        "IDENTIFIER",
        "OPERATOR",
        "WHITESPACE",
        "POTENTIAL_DOUBLE",
        "POTENTIAL_COMMENT",
        "COMMENT",
        "LOGICAL_EXPRESSION",
        "BINARY_EXPRESSION",
    };
    
    class Token
    {
        public:
            string text;
            uint16_t line_number;
            enum TokenType type {WHITESPACE};
            void debug_print() const;
        private:
    };

    class Tokenizer
    {
        public: 
            vector<Token> parse(const std::string &code);
        private:
            void end_token(Token &token, vector<Token> &tokens);
    };
}