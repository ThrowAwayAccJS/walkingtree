#include <stdexcept>
#include <iostream>
#include "Tokenizer.h"

namespace parser
{
    using namespace std;

    vector<Token> Tokenizer::parse(const std::string& code)
    {
        vector<Token> tokens;
        Token current_token;
        current_token.line_number = 1;

        for (size_t i = 0; i < code.length(); i++)
        {
            char curr_ch = code[i];

            if (current_token.type == COMMENT)
            {
                if (curr_ch != '\r' && curr_ch != '\n')
                {
                    continue;
                }
            }

            switch(curr_ch)
            {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                	if (current_token.type == WHITESPACE)
                    {
                        current_token.type = INTEGER_LITERAL;
                        current_token.text.append(1, curr_ch);
                    }
                    else if (current_token.type == POTENTIAL_DOUBLE)
                    {
                        current_token.type = DOUBLE_LITERAL;
                        current_token.text.append(1, curr_ch);
                    }
                    else
                    {
                        current_token.text.append(1, curr_ch);
                    }
                    break;
                case '=':
                    if (current_token.type != STRING_LITERAL)
                    {
                        end_token(current_token, tokens);
                        if (code[i+1] == curr_ch)
                        {
                            current_token.text.append(2, curr_ch);
                            current_token.type = OPERATOR;
                            i++;
                            end_token(current_token, tokens);
                        }
                        else
                        {
                            current_token.text.append(1, curr_ch);
                            current_token.type = OPERATOR;
                            end_token(current_token, tokens);
                        }
                    }
                    else 
                    {
                        current_token.text.append(1, curr_ch);
                    }
                    break;
                case '+':
                case '*':
                case '-':
                    if (current_token.type != STRING_LITERAL)
                    {
                        end_token(current_token, tokens);
                        if (code[i+1] == '=')
                        {
                            current_token.text.append(1, curr_ch);
                            current_token.text.append(1, code[i+1]);
                            current_token.type = OPERATOR;
                            i++;
                            end_token(current_token, tokens);
                        }
                        else 
                        {                          
                            current_token.text = curr_ch;
                            current_token.type = OPERATOR;
                            end_token(current_token, tokens);
                        }
                    }
                    else
                    {
                        current_token.text.append(1, curr_ch);
                    }
                    break;
                case '>':
                case '<':
                case '{':
                case '}':
                case '(':
                case ')':
                case ';':
                case '[':
                case ']':
                case '%':
                case ',':
                    if (current_token.type != STRING_LITERAL)
                    {
                        end_token(current_token, tokens);
                        current_token.text = curr_ch;
                        current_token.type = OPERATOR;
                        end_token(current_token, tokens);
                    }
                    else
                    {
                        current_token.text.append(1, curr_ch);
                    }
                    break;
                case '&':
                case '|':
                    if (current_token.type != STRING_LITERAL)
                    {
                        end_token(current_token, tokens);
                        if (code[i+1] != curr_ch)
                        {
                            throw runtime_error(string("Expected '") + curr_ch + string("' found '") + code[i+1] + string("'."));
                        }

                        current_token.text.append(2, curr_ch);
                        current_token.type = OPERATOR;
                        i++;
                        end_token(current_token, tokens);
                    }
                    else 
                    {
                        current_token.text.append(1, curr_ch);
                    }
                    break;
                case '.':
                	if (current_token.type == WHITESPACE)
                    {
                        current_token.type = POTENTIAL_DOUBLE;
                        current_token.text.append(1, curr_ch);
                    }
                    else if (current_token.type == INTEGER_LITERAL)
                    {
                        current_token.type = DOUBLE_LITERAL;
                        current_token.text.append(1, curr_ch);
                    }
                    else if (current_token.type == STRING_LITERAL)
                    {
                        current_token.text.append(1, curr_ch);
                    }
                    else if (current_token.type != COMMENT)
                    {
                        end_token(current_token, tokens);
                        current_token.type = OPERATOR;
                        current_token.text.append(1, curr_ch);
                        end_token(current_token, tokens);
                    }
                    break;
                case '"':
                    if (current_token.type != STRING_LITERAL)
                    {
                        end_token(current_token, tokens);
                        current_token.type = STRING_LITERAL;
                    }
                    else
                    {
                        end_token(current_token, tokens);
                    }
                    break;
                case ' ':
                    if (current_token.type == STRING_LITERAL)
                    {
                        current_token.text.append(1, curr_ch);
                    } 
                    else
                    {
                        end_token(current_token, tokens);
                    }
                    break;
                case '\t':
                    break;
                case '\r':
                case '\n':
                    end_token(current_token, tokens);
                    current_token.line_number++;
                    break;
                case '/':
                    if (current_token.type == STRING_LITERAL)
                    {
                        current_token.text.append(1, curr_ch);
                    }
                    else if (current_token.type == POTENTIAL_COMMENT)
                    {
                        current_token.type = COMMENT;
                        current_token.text.erase();
                    }
                    else if (current_token.type != COMMENT)
                    {
                        end_token(current_token, tokens);
                        current_token.type = POTENTIAL_COMMENT;
                        current_token.text.append(1, curr_ch);
                    }
                    break;
                default:
                    if (current_token.type == WHITESPACE 
                        || current_token.type == INTEGER_LITERAL 
                        || current_token.type == DOUBLE_LITERAL
                        || current_token.type == POTENTIAL_DOUBLE)
                    {
                        end_token(current_token, tokens);
                        current_token.type = IDENTIFIER;
                        current_token.text.append(1, curr_ch);
                    }
                    else
                    {
                        current_token.text.append(1, curr_ch);
                    }
                    break;
            }
        }

        end_token(current_token, tokens);

        return tokens;
    }

    void Tokenizer::end_token(Token &token, vector<Token> &tokens)
    {
        if (token.type == POTENTIAL_DOUBLE)
		{
			if (token.text.compare(".") == 0)
			{
				token.type = OPERATOR;
			}
			else
			{
				token.type = DOUBLE_LITERAL;
			}
		}

        if (token.type != WHITESPACE && token.type != COMMENT)
        {
            tokens.push_back(token);
        }

        token.type = WHITESPACE;
        token.text.erase();
    }

    void Token::debug_print() const
    {
        cout << token_type_strings[type] << " \"" << text << "\"" << " Line Number: " << line_number << endl;
    }
}