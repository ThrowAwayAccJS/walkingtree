#include "Parser.h"
#include <iostream>

namespace parser
{
    using namespace std;

    struct OperatorEntry
	{
		std::string m_name;
		size_t m_precendence;
	};

    static std::vector<std::string> equal_operators = { "=", "+=", "-=", "*=", "/=" };
    static std::vector<std::string> prefix_operators = { "-", "+", "!" };

	static std::unordered_map<std::string, OperatorEntry> operators
	{
		// 0 reserved for no operator
		{ "+", OperatorEntry{"+", 5} },
		{ "-", OperatorEntry{"-", 5} },
		{ "&&", OperatorEntry{"&&", 1} },
        { "||", OperatorEntry{"||", 1} },
        { "==", OperatorEntry{"==", 2}},
		{ ">", OperatorEntry{">", 2} },
        { "<", OperatorEntry{"<", 2} },
        { "%", OperatorEntry{"%", 3}},
		{ "*", OperatorEntry{"*", 10} },
		{ "/", OperatorEntry{"/", 10} }
	};

    Parser::Parser()
    {
        types["string"] = Type("string", STRING);
        types["int"] = Type("int", INTEGER);
        types["double"] = Type("double", DOUBLE);
        types["bool"] = Type("bool", BOOLEAN);
        types["void"] = Type("void", VOID);
        types["vector"] = Type("vector", VECTOR);
    }

    bool Parser::expect_function_definition()
    {
        vector<Token>::iterator parse_start = current_token;
        optional<Type> potential_type = expect_type();
        if (potential_type.has_value())
        {
            optional<Token> potential_name = expect_identifer();
            if (potential_name.has_value())
            {
                optional<Token> potential_operator = expect_operator("(");
                if (potential_operator.has_value())
                {
                    FunctionDefinition function;
                    function.name = potential_name->text;
                    function.type = potential_type->type;

                    while(!expect_operator(")").has_value())
                    {
                        optional<Type> possible_parameter_type = expect_type();
                        if (!possible_parameter_type.has_value())
                        {
                            throw runtime_error("Expected a type, got: '" + current_token->text + "'.");
                        }

                        optional<Token> possible_parameter_name = expect_identifer();
                        if (!possible_parameter_type.has_value())
                        {
                            throw runtime_error("Expected a name definition, got: '" + current_token->text + "'.");
                        }

                        ParameterDefinition paramemeter_definition;
                        paramemeter_definition.name = possible_parameter_name->text;
                        paramemeter_definition.type = possible_parameter_type.value();
                        function.parameters.push_back(paramemeter_definition);

                        if (expect_operator(")").has_value())
                        {
                            break;
                        }

                        if (!expect_operator(","))
                        {
                            throw runtime_error("Expected ',' to separate function parameters, got '" + current_token->text + "'.");
                        }
                    }

                    optional<vector<Statement>> statements = parse_function_body();

                    if (!statements.has_value())
                    {
                        current_token = parse_start;
                        return false;
                    }

                    function.statements.insert(function.statements.begin(), statements->begin(), statements->end());
                    functions[function.name] = function;

                    return true;
                }
                else
                {
                    
                }
            }
            else 
            {
                
            }
        }
        else
        {
            
        }

        current_token = parse_start;
        return false;
    }

    bool Parser::expect_external_function_definition()
    {
        if (!expect_identifer("extern").has_value())
        {
            return false;
        }

        optional<Type> potential_type = expect_type();
        if (potential_type.has_value())
        {
            optional<Token> potential_name = expect_identifer();
            if (potential_name.has_value())
            {
                optional<Token> potential_operator = expect_operator("(");
                if (potential_operator.has_value())
                {
                    FunctionDefinition function;
                    function.name = potential_name->text;
                    function.type = potential_type->type;

                    while(!expect_operator(")").has_value())
                    {
                        optional<Type> possible_parameter_type = expect_type();
                        if (!possible_parameter_type.has_value())
                        {
                            throw runtime_error("Expected a type, got: '" + current_token->text + "'.");
                        }

                        optional<Token> possible_parameter_name = expect_identifer();
                        if (!possible_parameter_type.has_value())
                        {
                            throw runtime_error("Expected a name definition, got: '" + current_token->text + "'.");
                        }

                        ParameterDefinition paramemeter_definition;
                        paramemeter_definition.name = possible_parameter_name->text;
                        paramemeter_definition.type = possible_parameter_type.value();
                        function.parameters.push_back(paramemeter_definition);

                        if (expect_operator(")").has_value())
                        {
                            break;
                        }

                        if (!expect_operator(","))
                        {
                            throw runtime_error("Expected ',' to separate function parameters, got '" + current_token->text + "'.");
                        }
                    }

                    if (!expect_operator(";"))
                    {
                        throw runtime_error("Expected ';' to end external function definition, got '" + current_token->text + "'.");
                    }

                    function.external = true;
                    functions[function.name] = function;
                    return true;
                }

            }
        }

        return false;
    }

    optional<vector<Statement>> Parser::parse_function_body()
    {
        if (!expect_operator("{").has_value())
        {
            return std::nullopt;
        }

        std::vector<Statement> statements;

        while (!expect_operator("}").has_value())
        {
            std::optional<Statement> statement = expect_statement();
            if (statement.has_value())
            {
                statements.push_back(statement.value());
            }
            else
            {
                std::cerr << "Expected statement, got: '" << current_token->text << "'." << std::endl;
            }
        }

        return statements;
    }

    optional<Statement> Parser::expect_statement()
    {
        optional<Statement> statement = expect_variable_declaration();
        if (statement.has_value())
        {
            if (expect_operator(";").has_value())
            {
                return statement;
            }

            throw runtime_error("Expected ';' at the end of statement. On line number " + std::to_string(current_token->line_number) + ".");
        }

        statement = expect_assignment();
        if (statement.has_value())
        {
            if (expect_operator(";").has_value())
            {
                return statement;
            }

            throw runtime_error("Expected ';' at the end of statement. On line number " + std::to_string(current_token->line_number) + ".");
        }

        if (current_token->type == IDENTIFIER)
        {
            optional<Token> potential_pre_built = expect_identifer("for");
            if (potential_pre_built.has_value())
            {
                statement = for_statement();
                return statement;
            }

            potential_pre_built = expect_identifer("if");
            if (potential_pre_built.has_value())
            {
                statement = if_statement();
                return statement;
            }
        }

        if (expect_identifer("return").has_value())
        {
            Statement return_statement;
            return_statement.kind = RETURN_STATEMENT;
            
            std::optional<Statement> expression = expect_expression();
            if (expression.has_value())
            {
                return_statement.parameters.push_back(expression.value());
            }

            if (expect_operator(";").has_value())
            {
                return return_statement;
            }

            throw runtime_error("Expected ';' at the end of a return statement");
        }

        statement = expect_function_call();
        if (statement.has_value())
        {
            if (expect_operator(";").has_value())
            {
                return statement;
            }

            throw runtime_error("Expected ';' at the end of statement. On line number " + std::to_string(current_token->line_number) + ".");
        }

        return nullopt;
    }

    optional<Statement> Parser::expect_function_call()
    {
        auto function_start = current_token;
        optional<Type> potential_type = expect_type();
        if (!potential_type.has_value())
        {
            current_token = function_start;
            optional<Token> potential_name = expect_identifer();
            if (potential_name.has_value())
            {
                if (expect_operator("(").has_value())
                {
                    Statement function_call;
                    function_call.kind = FUNCTION_CALL;
                    function_call.name = potential_name->text;
                    while(!expect_operator(")").has_value())
                    {
                        optional<Statement> potential_identifer = expect_expression();
                        if (potential_identifer.has_value())
                        {
                            function_call.parameters.push_back(potential_identifer.value());
                            if (expect_operator(")").has_value())
                            {
                                break;
                            }

                            if (!expect_operator(",").has_value())
                            {
                                throw runtime_error("Expected ',' to separate parameters inside the function");
                            }
                        }
                        else 
                        {
                            throw runtime_error("Expected either a parameter or ')' during function call.");
                        }
                    }

                    return function_call;
                }
                else 
                {
                    
                }
            }
        }

        return nullopt;
    }

    optional<Statement> Parser::expect_assignment()
    {
        auto assignment_start = current_token;
        optional<Type> potential_type = expect_type();
        if (!potential_type.has_value())
        {
            current_token = assignment_start;
            optional<Token> potential_name = expect_identifer();
            if (potential_name.has_value())
            {
                Statement statement;
                statement.kind = ASSIGNMENT_EXPRESSION;
                statement.name = potential_name->text;

                std::optional<Statement> index_access = expect_index_access(true);

                if (index_access.has_value())
                {   
                    index_access->kind = ELEMENT_ACCESS_EXPRESSION;
                    statement.parameters.push_back(index_access.value());
                }

                for (auto equal_operator : equal_operators)
                {
                    optional<Token> potential_operator = expect_operator(equal_operator);
                    if (potential_operator.has_value())
                    {
                        optional<Statement> potential_expression = expect_expression();
                        if (potential_expression.has_value())
                        {
                            if (equal_operator == "+=") statement.kind = ASSIGNMENT_PLUS_EXPRESSION;
                            else if (equal_operator == "-=") statement.kind = ASSIGNMENT_MINUS_EXPRESSION;
                            else if (equal_operator == "*=") statement.kind = ASSIGNMENT_MULTIPLY_EXPRESSION;
                            else if (equal_operator == "/=") statement.kind = ASSIGNMENT_DIVIDE_EXPRESSION;

                            statement.parameters.push_back(potential_expression.value());
                            return statement;
                        }
                        else 
                        {
                            throw runtime_error("Expected an expression after '" + equal_operator + "'.");
                        }
                    }
                }
            }
        }

        current_token = assignment_start;
        return nullopt;
    }

    std::optional<Statement> Parser::expect_variable_declaration()
    {
        auto declaration_start = current_token;
        std::optional<Type> potential_type = expect_type();
        if (potential_type.has_value())
        {
            std::optional<Token> potential_name = expect_identifer();
            if (potential_name.has_value())
            {
                Statement statement;
                statement.kind = VARIABLE_DECLARATION;
                statement.type = potential_type.value(); 
                statement.name = potential_name->text;

/*                 if (expect_index_access(true).has_value())
                {
                    Statement index_access_statement;
                    index_access_statement.kind = ELEMENT_ACCESS_EXPRESSION;
                } */

                std::optional<Token> potential_operator = expect_operator("=");
                if (potential_operator.has_value())
                {
                    optional<Statement> initial_value = expect_expression();
                    if (initial_value.has_value())
                    {
                        statement.parameters.push_back(initial_value.value());

                        return statement;
                    }

                    throw runtime_error("Expected an initial value when declaring a variable with '='");
                }

                return statement;
            }
        }

        current_token = declaration_start;
        return std::nullopt;
    }

    std::optional<Statement> Parser::array_literal_expression()
    {
        if (!expect_operator("{").has_value())
        {
            return std::nullopt;
        }

        Statement statement;
        statement.kind = VECTOR_LITERAL;

        if (expect_operator("}").has_value())
        {
            return statement;
        }

        while (true)
        {
            std::optional<Statement> potential_expression = expect_expression();
            if (!potential_expression.has_value())
            {
                throw std::runtime_error("Expected an expression inside an array literal.");
            }

            statement.parameters.push_back(potential_expression.value());

            if (expect_operator("}").has_value())
            {
                break;
            }

            if (!expect_operator(",").has_value())
            {
                throw std::runtime_error("Expected ',' to separate expressions inside an array literal.");
            }
        }

        return statement;
    }

    std::optional<Statement> Parser::expect_expression()
    {
        // ! will need to be reworked for objects
        std::optional<Statement> potential_array_literal = array_literal_expression();
        if (potential_array_literal.has_value())
        {
            return potential_array_literal;
        }

        std::optional<Statement> lhs = expect_one_value();
        if (!lhs.has_value())
        {
            current_token--;
            std::optional<Token> potential_identifier = expect_identifer();
            if (potential_identifier.has_value())
            {
                Statement statement;
                statement.kind = VARIABLE;
                statement.name = potential_identifier->text;

                std::optional<Statement> potential_index = expect_index_access(true);
                if (potential_index.has_value())
                {
                    potential_index->parameters.push_back(statement);
                    lhs = potential_index;
                }
                else 
                {
                    lhs = statement;
                }
            }
            else
            {
                return nullopt;
            }
        }

        std::optional<Statement> property_access = expect_property_access(lhs.value());

        if (property_access.has_value())
        {
            lhs = property_access.value();
        }

        while (true)
		{
			std::optional<Token> op = expect_operator();
			if (!op.has_value())
			{
                break;
			}

			int rhs_precedence = operator_precedence(op->text);

			if (rhs_precedence == 0)
			{
				--current_token;
				return lhs;
			}

			std::optional<Statement> rhs = expect_one_value();
			if (!rhs.has_value())
			{
                current_token--;
                optional<Token> potential_identifier = expect_identifer();
                if (potential_identifier.has_value())
                {
                    Statement statement;
                    statement.kind = VARIABLE;
                    statement.name = potential_identifier->text;

                    std::optional<Statement> potential_index = expect_index_access(true);
                    if (potential_index.has_value())
                    {
                        potential_index.value().parameters.push_back(statement);
                        rhs = potential_index;
                    }
                    else 
                    {
                        rhs = statement;
                    }
                }
                else
                {
				    return lhs;
                }
			}

            std::optional<Statement> property_access = expect_property_access(rhs.value());

            if (property_access.has_value())
            {
                rhs = property_access.value();
            }

			Statement* rightmost_statement = find_rightmost_statement(&lhs.value(), rhs_precedence);
			if (rightmost_statement)
			{
				Statement operator_call;
				operator_call.kind = StatementKind::OPERATOR_CALL;
				operator_call.name = op->text;
				operator_call.parameters.push_back(rightmost_statement->parameters.at(1));
				operator_call.parameters.push_back(rhs.value());
				rightmost_statement->parameters[1] = operator_call;
			}
			else
			{
				Statement operator_call;
				operator_call.kind = StatementKind::OPERATOR_CALL;
				operator_call.name = op->text;
				operator_call.parameters.push_back(lhs.value());
				operator_call.parameters.push_back(rhs.value());
				lhs = operator_call;
			}
		}

		return lhs;
    }

    optional<Statement> Parser::expect_one_value()
    {
        std::optional<Statement> result;

        if (expect_operator("-").has_value())
        {
            Statement statement;
            statement.kind = StatementKind::OPERATOR_CALL;
            statement.name = "-";
            statement.parameters.push_back(expect_one_value().value());
            result = statement;
            
            return result;
        }

        if (current_token != end_token && current_token->type == STRING_LITERAL)
        {
            Statement string_literal_statement;
            string_literal_statement.type = Type("string", STRING);
            string_literal_statement.name = current_token->text;
            string_literal_statement.kind = LITERAL;
            result = string_literal_statement;
            current_token++;
        }
        else if (current_token != end_token && (current_token->text == "true" || current_token->text == "false"))
        {
            Statement string_literal_statement;
            string_literal_statement.type = Type("bool", BOOLEAN);
            string_literal_statement.name = current_token->text;
            string_literal_statement.kind = LITERAL;
            result = string_literal_statement;
            current_token++;
        }
        else if (current_token != end_token && current_token->type == INTEGER_LITERAL)
        {
            Statement integer_literal_statement;
            integer_literal_statement.type = Type("integer", INTEGER);
            integer_literal_statement.name = current_token->text;
            integer_literal_statement.kind = LITERAL;
            result = integer_literal_statement;

            current_token++;
        }
        else if (current_token != end_token && current_token->type == DOUBLE_LITERAL)
        {
            Statement double_literal_statement;
            double_literal_statement.type = Type("double", DOUBLE);
            double_literal_statement.name =  current_token->text;
            double_literal_statement.kind = LITERAL;
            result = double_literal_statement;

            current_token++;
        }

        if (!result.has_value())
        {
            result = expect_function_call();
        }

        std::vector<parser::Token>::iterator parse_start = current_token;
        std::optional<Statement> potential_index = expect_index_access(true);

        if (potential_index.has_value() && result.has_value())
        {
            potential_index.value().parameters.push_back(result.value());
            result = potential_index.value();
            return result;
        }

        current_token = parse_start;

        return result;
    }

    std::optional<Statement> Parser::expect_property_access(Statement &other)
    {
        if (!expect_operator(".").has_value())
        {
            return std::nullopt;
        }

        std::optional<Token> identifier = expect_identifer();

        if (!identifier.has_value())
        {
            throw std::runtime_error("Expected identifier after '.' on line number " + std::to_string(current_token->line_number));
        }

        Statement first_property_access;
        first_property_access.kind = PROPETY_ACCESS_EXPRESSION;
        first_property_access.name = identifier->text;
        first_property_access.parameters.push_back(other);

        while (true)
        {
            if (!expect_operator(".").has_value())
            {
                return first_property_access;
            }

            std::optional<Token> identifier = expect_identifer();

            if (!identifier.has_value())
            {
                throw std::runtime_error("Expected identifier after '.' on line number " + std::to_string(current_token->line_number));
            }

            Statement property_access;
            property_access.kind = PROPETY_ACCESS_EXPRESSION;
            property_access.name = identifier->text;

            property_access.parameters.push_back(first_property_access);
            first_property_access = property_access;
        }
    }

    std::optional<Statement> Parser::expect_index_access(const bool with_value)
    {
        std::vector<parser::Token>::iterator parse_start = current_token;
        if (!expect_operator("[").has_value())
        {
            return std::nullopt;
        }

        std::optional<Statement> potential_expression = expect_expression();

        if (!potential_expression.has_value() && with_value)
        {
            current_token = parse_start;
            return std::nullopt;
        }

        Statement statement;
        statement.kind = INDEX;

        if (potential_expression.has_value())
        {
            statement.parameters.push_back(potential_expression.value());
        }

        if (!expect_operator("]").has_value())
        {
            throw runtime_error("Expected a ']' operator to close an index on line " + current_token->line_number);
        }

        return statement;
    }

    std::optional<Token> Parser::expect_identifer(const string &name)
    {
        if (current_token == end_token)
        {
            return std::nullopt;
        }

        if (current_token->type != IDENTIFIER)
        {
            return std::nullopt;
        }

        if (!name.empty() && current_token->text != name)
        {
            return std::nullopt;
        }

        Token token = *current_token;
        current_token++;

        return token;
    }

    optional<Type> Parser::expect_type()
    {
        std::optional<Token> potential_identifier = expect_identifer();
        if (!potential_identifier.has_value())
        {
            return std::nullopt;
        }

        std::unordered_map<string, Type>::iterator type = types.find(potential_identifier->text);
        if (type == types.end())
        {
            return std::nullopt;
        }

        return type->second;
    }

    optional<Token> Parser::expect_operator(const string &name)
    {
        if (current_token == end_token)
        {
            return std::nullopt;
        }

        if (current_token->type != OPERATOR)
        {
            return std::nullopt;
        }

        if (!name.empty() && current_token->text != name)
        {
            return std::nullopt;
        }

        Token token = *current_token;
        current_token++;

        return token;
    }

    size_t Parser::operator_precedence(const std::string& operator_name) 
	{
		std::unordered_map<std::string, OperatorEntry>::iterator found_operator = operators.find(operator_name);
		
		if (found_operator == operators.end())
		{
			return 0;
		}

		return found_operator->second.m_precendence;
	}

	Statement* Parser::find_rightmost_statement(Statement *lhs, size_t rhs_precedence)
	{
		if (lhs->kind != StatementKind::OPERATOR_CALL)
		{
			return nullptr;
		}

		if (operator_precedence(lhs->name) >= rhs_precedence)
		{
			return nullptr;
		}

		Statement* rhs = &lhs->parameters.at(1);
		if (rhs->kind != StatementKind::OPERATOR_CALL)
		{
			return lhs;
		}

		rhs = find_rightmost_statement(rhs, rhs_precedence);
		if (rhs == nullptr)
		{
			return lhs;
		}

		return rhs;
	}

    optional<Statement> Parser::for_statement()
    {
        optional<Token> potential_operator = expect_operator("(");
        if (!potential_operator.has_value())
        {
            throw runtime_error("Expected '(' at the start of for loop conditions.");
        }

        optional<Statement> potential_initial_statement = expect_variable_declaration();
        if (!potential_initial_statement.has_value())
        {
            potential_initial_statement = expect_assignment();
            if (!potential_initial_statement.has_value())
            {
                throw runtime_error("Expected either a variable declaration, or assignment");
            }
        }

        if (!expect_operator(";").has_value())
        {
            throw runtime_error("Expected ';' at the end of statement. On line number " + std::to_string(current_token->line_number) + ".");
        }

        optional<Statement> potential_expression = expect_expression();
        if (!potential_expression.has_value())
        {
            throw runtime_error("Expected an expression inside a for loop.");
        }

        if (!expect_operator(";").has_value())
        {
            throw runtime_error("Expected ';' at the end of statement.");
        }

        optional<Statement> potential_assignment = expect_assignment();
        if (!potential_assignment.has_value())
        {
            throw runtime_error("Expected an assignment inside a for loop.");
        }

        potential_operator = expect_operator(")");
        if (!potential_operator.has_value())
        {
            throw runtime_error("Expected ')' at the end of for loop conditions.");
        }

        optional<vector<Statement>> statements = parse_function_body();

        Statement body;
        body.kind = BLOCK_STATEMENT;
        body.parameters = statements.value();

        if (!statements.has_value())
        {
            throw runtime_error("Expected a body for a 'for' loop.");
        }

        Statement statement;
        statement.parameters.push_back(potential_initial_statement.value());
        statement.parameters.push_back(potential_expression.value());
        statement.parameters.push_back(potential_assignment.value());
        statement.parameters.push_back(body);
        statement.kind = FOR_STATEMENT;

        return statement;
    }

    optional<Statement> Parser::if_statement()
    {
        if (!expect_operator("(").has_value())
        {
            return std::nullopt;
        }

        std::optional<Statement> potential_expression = expect_expression();
        
        if (!potential_expression.has_value())
        {
            throw runtime_error("Expected an expression inside an if statement.");
        }

        if (!expect_operator(")").has_value())
        {
            return std::nullopt;
        }

        optional<vector<Statement>> statements = parse_function_body();

        if (!statements.has_value())
        {
            return std::nullopt;
        }

        Statement body;
        body.kind = BLOCK_STATEMENT;
        body.parameters = statements.value();

        Statement statement;
        statement.parameters.push_back(potential_expression.value());
        statement.parameters.push_back(body);
        statement.kind = IF_STATEMENT;

        if (expect_identifer("else").has_value())
        {
            if (expect_identifer("if").has_value())
            {
                std::optional<Statement> block = if_statement();
                if (block.has_value())
                {
                    statement.parameters.push_back(block.value());
                }
                else
                {
                    throw runtime_error("Expected an else if.");
                }
            }
            else
            {
                optional<vector<Statement>> statements = parse_function_body();
                if (statements.has_value())
                {
                    Statement body;
                    body.kind = BLOCK_STATEMENT;
                    body.parameters = statements.value();

                    statement.parameters.push_back(body);

                    return statement;
                }
                else
                {
                    throw runtime_error("Expected a block statement to follow an else clause");
                }
            }
        }

        return statement;
    }

    optional<Statement> else_statement()
    {
        return nullopt;
    }

    std::unordered_map<string, FunctionDefinition> Parser::parse(vector<Token> &tokens)
    {
        end_token = tokens.end();
        current_token = tokens.begin();

        while(current_token != end_token)
        {
            if (!expect_function_definition())
            {
                if (!expect_external_function_definition())
                {
                    current_token++;
                }
            }
            else
            {
                
            }
        }

        return functions;
    }

    void Parser::debug_print() const
    {
        for (auto function : functions)
        {
            function.second.debug_print();
            std::cout << std::endl;
        }
    }
}