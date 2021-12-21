#include "Interpreter.h"
#include <iostream>
#include <chrono>
#include <Windows.h>
#include <thread>

namespace parser
{
    std::optional<std::shared_ptr<VariableType>> cout_println(unordered_map<std::string, std::shared_ptr<VariableType>> &variables_in_current_scope)
    {
        std::cout << variables_in_current_scope["arg"]->GetValue() << std::endl;
        return std::nullopt;
    }

    std::optional<std::shared_ptr<VariableType>> cout_print(unordered_map<std::string, std::shared_ptr<VariableType>> &variables_in_current_scope)
    {
        std::cout << variables_in_current_scope["arg"]->GetValue();
        return std::nullopt;
    }

    std::optional<std::shared_ptr<VariableType>> cin_read(unordered_map<std::string, std::shared_ptr<VariableType>> &variables_in_current_scope)
    {
        std::string input;

        std::cin >> input;
        return std::make_shared<String>(input);
    }

    std::optional<std::shared_ptr<VariableType>> clear_console(unordered_map<std::string, std::shared_ptr<VariableType>> &variables_in_current_scope)
    {
        system("cls");
        return std::nullopt;
    }

    std::optional<std::shared_ptr<VariableType>> time(unordered_map<std::string, std::shared_ptr<VariableType>> &variables_in_current_scope)
    {
        return std::make_shared<Integer>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    }

    std::optional<std::shared_ptr<VariableType>> get_key_state(unordered_map<std::string, std::shared_ptr<VariableType>> &variables_in_current_scope)
    {
        switch (std::stoi(variables_in_current_scope["arg"]->GetValue()))
        {
            case 57:
                return std::make_shared<Boolean>(GetAsyncKeyState(0x57) & 1);
            case 53:
                return std::make_shared<Boolean>(GetAsyncKeyState(0x53) & 1);
            case 41:
                return std::make_shared<Boolean>(GetAsyncKeyState(0x41) & 1);
            case 44:
                return std::make_shared<Boolean>(GetAsyncKeyState(0x44) & 1);
            default:
                return std::make_shared<Boolean>(false);
        }
    }

    std::optional<std::shared_ptr<VariableType>> rand_int(unordered_map<std::string, std::shared_ptr<VariableType>> &variables_in_current_scope)
    {
        return std::make_shared<Integer>(rand() % std::stoi(variables_in_current_scope["arg"]->GetValue()));
    }

    std::optional<std::shared_ptr<VariableType>> sleep_for(unordered_map<std::string, std::shared_ptr<VariableType>> &variables_in_current_scope)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(std::stoi(variables_in_current_scope["arg"]->GetValue())));
        return std::nullopt;
    }

    Interpreter::Interpreter(std::unordered_map<std::string, FunctionDefinition> &functions)
    {
        this->functions = functions;
        external_functions["println"] = cout_println;
        external_functions["print"] = cout_print;
        external_functions["readln"] = cin_read;
        external_functions["time"] = time;
        external_functions["clear_console"] = clear_console;
        external_functions["get_key_state"] = get_key_state;
        external_functions["rand_int"] = rand_int;
        external_functions["sleep"] = sleep_for;
    }

    std::optional<std::shared_ptr<VariableType>> Interpreter::Parse(std::vector<Statement> args, std::string entry_point)
    {
        if (variables_in_current_scope.size())
        {
            scope_start.push_back(variables_in_current_scope.begin()->first);
        }

        if (args.size() < functions[entry_point].parameters.size())
        {
            std::cerr << "\nNot enough arguments for function '" << entry_point << "'" << std::endl;
            return std::make_unique<Integer>(-1);
        }

        for (size_t i = 0; i < functions[entry_point].parameters.size(); i++)
        {
            #define function_parameter functions[entry_point].parameters[i]
            // pre existing variables, if true loop continues, if not carries on.
            if (variables_in_current_scope.find(function_parameter.name) != variables_in_current_scope.end())
            {
                variables_in_current_scope[function_parameter.name] = *descent(args[i]);
                continue;
            }

            if (function_parameter.type.type != args[i].type.type && !functions[entry_point].external)
            {
                std::cerr << "\nError: parameter " << i << " of function " << entry_point << " has wrong type" << std::endl; 
                return std::make_unique<Integer>(-1);
            }

            switch (functions[entry_point].parameters[i].type.type)
            {
                case STRING:
                    variables_in_current_scope[function_parameter.name] = std::make_shared<String>("");
                    *variables_in_current_scope[function_parameter.name] = *descent(args[i]).value();
                    break;
                case INTEGER:
                    variables_in_current_scope[function_parameter.name] = std::make_shared<Integer>(0);
                    *variables_in_current_scope[function_parameter.name] = *descent(args[i]).value();
                    break;
                case BOOLEAN:
                    variables_in_current_scope[function_parameter.name] = std::make_shared<Boolean>(false);
                    *variables_in_current_scope[function_parameter.name] = *descent(args[i]).value();
                    break;
                default:
                    std::cerr << "\nTYPE NOT IMPLEMETED YET" << std::endl;
            }
        }

        if (functions[entry_point].external)
        {
            std::optional<std::shared_ptr<parser::VariableType>> potential_return_value = external_functions[entry_point](variables_in_current_scope);
            clear_scope();
            return potential_return_value;
        }

        std::vector<parser::Statement> statements = functions[entry_point].statements;

        for (size_t i = 0; i < statements.size(); i++)
        {
            std::optional<std::shared_ptr<parser::VariableType>> potential_return = descent(statements[i]);
            if (potential_return.has_value())
            {
                if (potential_return.value()->GetType() != functions[entry_point].type)
                {
                    throw std::runtime_error("Return type of '" + entry_point + "' is not the same as the function's return type");
                }

                clear_scope();
                return potential_return.value();
            }
        }
        
        clear_scope();
        return std::nullopt;
    }

    // using std::optional because a function can return a value or void
    // descent function used to traverse the AST and execute the code, takes in the top node of the AST 
    // and works its way down to the leaves
    std::optional<std::shared_ptr<VariableType>> Interpreter::descent(Statement &statement)
    {
        if (!statement.parameters.size())
        {
            switch(statement.kind)
            {
                case LITERAL:
                    if (statement.type.type == INTEGER)
                    {
                        return std::make_shared<Integer>(std::stoi(statement.name));
                    }
                    else if (statement.type.type == STRING)
                    {
                        return std::make_shared<String>(statement.name);
                    }
                    else if (statement.type.type == BOOLEAN)
                    {
                        return std::make_shared<Boolean>(statement.name == "true" ? true : false);
                    }
                    break;
                case VARIABLE:
                    if (variables_in_current_scope.find(statement.name) == variables_in_current_scope.end())
                    {
                        throw std::runtime_error("\nInterpreter Error: Variable '" + statement.name + "' not defined.");
                    }

                    return variables_in_current_scope[statement.name];
                    break;
                case VARIABLE_DECLARATION:
                    if (variables_in_current_scope.find(statement.name) == variables_in_current_scope.end())
                    {  
                        if (statement.type.type == INTEGER)
                        {
                            variables_in_current_scope[statement.name] = std::make_unique<Integer>(0);
                        }
                        else if (statement.type.type == STRING)
                        {
                            variables_in_current_scope[statement.name] = std::make_unique<String>("");
                        }
                        else if (statement.type.type == BOOLEAN)
                        {
                            variables_in_current_scope[statement.name] = std::make_unique<Boolean>(false);
                        }

                        return std::nullopt;
                    }
                    else
                    {
                        throw runtime_error("\nInterpreter Error: Variable '" + statement.name + "' already defined.");
                    }
                    break;
                case FUNCTION_CALL:
                    if (functions.find(statement.name) == functions.end())
                    {
                        throw runtime_error("\nInterpreter Error: Function '" + statement.name + "' not defined.");
                    }
                    else 
                    {
                        std::optional<std::shared_ptr<parser::VariableType>> potential_return_value = Parse({}, statement.name);
                        if (potential_return_value.has_value())
                        {
                            return potential_return_value.value();
                        }
                        else 
                        {
                            return std::nullopt;
                        }
                    }
                    break;
                case RETURN_STATEMENT:
                    return std::nullopt;
                    break;
                default:
                    std::cerr << "\nInterpreter Error: Statement not implemented yet" << std::endl;
                    return std::nullopt;
            }
        }

        switch (statement.kind)
        {
            case PROPETY_ACCESS_EXPRESSION:
                return descent(statement.parameters.front()).value()->property_access(statement.name);
                break;
            case FOR_STATEMENT:
                if (variables_in_current_scope.size())
                {
                    scope_start.push_back(variables_in_current_scope.begin()->first);
                }

                descent(statement.parameters[0]);

                while (*descent(statement.parameters[1]).value())
                {
                    if (variables_in_current_scope.size())
                    {
                        scope_start.push_back(variables_in_current_scope.begin()->first);
                    }

                    for (size_t i = 0; i < statement.parameters[3].parameters.size(); i++)
                    {
                        descent(statement.parameters[3].parameters[i]);
                    }

                    clear_scope();

                    descent(statement.parameters[2]);
                }

                clear_scope();

                break;
            case INDEX:
                return descent(statement.parameters[1]).value()->operator[](*descent(statement.parameters[0]).value());
                break;
            case ELEMENT_ACCESS_EXPRESSION: {
                return descent(statement.parameters[0]).value();
                break;
/*                 auto temp = descent(statement.parameters[0]).value();
                switch (temp->GetType())
                {
                    case INTEGER:
                        return std::make_shared<Integer>(std::stoi(temp->GetValue()));
                    case STRING:
                        return std::make_shared<String>(temp->GetValue());
                    case BOOLEAN:
                        return std::make_shared<Boolean>(temp->GetValue() == "true" ? true : false);
                    default: 
                        throw std::runtime_error("Type not implemented yet, 240");
                } */
            }
            case VECTOR_LITERAL: {
                std::vector<std::shared_ptr<VariableType>> vect;
                for (size_t i = 0; i < statement.parameters.size(); i++)
                {
                    if (statement.parameters[i].kind == VARIABLE)
                    {
                        auto temp = descent(statement.parameters[i]).value();
                        switch (temp->GetType())
                        {
                            case INTEGER:
                                vect.push_back(std::make_shared<Integer>(std::stoi(descent(statement.parameters[i]).value()->GetValue())));
                                break;
                            case STRING:
                                vect.push_back(std::make_shared<String>(descent(statement.parameters[i]).value()->GetValue()));
                                break;
                            case BOOLEAN:
                                vect.push_back(std::make_shared<Boolean>(descent(statement.parameters[i]).value()->GetValue() == "true" ? true : false));
                                break;
                            default:
                                throw std::runtime_error("Type not implemented yet.");
                        }
                    } 
                    else 
                    {
                        vect.push_back(descent(statement.parameters[i]).value());
                    }
                }

                return std::make_shared<Vector>(vect);
                break;
            }
            case IF_STATEMENT:
                if (*descent(statement.parameters[0]).value())
                {
                    if (variables_in_current_scope.size())
                    {
                        scope_start.push_back(variables_in_current_scope.begin()->first);
                    }

                    for (Statement statement_in_block : statement.parameters[1].parameters)
                    {
                        descent(statement_in_block);
                    }

                    clear_scope();
                }
                else if (statement.parameters[2].kind == BLOCK_STATEMENT)
                {
                    if (variables_in_current_scope.size())
                    {
                        scope_start.push_back(variables_in_current_scope.begin()->first);
                    }

                    for (Statement statement_in_block : statement.parameters[2].parameters)
                    {
                        descent(statement_in_block);
                    }

                    clear_scope();
                }
                else if (statement.parameters[2].kind == IF_STATEMENT)
                {
                    if (variables_in_current_scope.size())
                    {
                        scope_start.push_back(variables_in_current_scope.begin()->first);
                    }

                    descent(statement.parameters[2]);

                    clear_scope();
                }
                break;
            case OPERATOR_CALL:
                // TODO ensure that both nodes have value
                // side effect can be an error "bad optional access"
                if (statement.name == "==")
                {
                    return std::make_unique<Boolean>((
                        *descent(statement.parameters[0]).value()
                        == *descent(statement.parameters[1]).value()));
                }
                else if (statement.name == "<")
                {
                    return std::make_unique<Boolean>(
                        *descent(statement.parameters[0]).value()
                        < *descent(statement.parameters[1]).value());
                }
                else if (statement.name == "+")
                {
                    return *descent(statement.parameters[0]).value() 
                        + *descent(statement.parameters[1]).value();
                }
                else if (statement.name == "-")
                {
                    if (statement.parameters.size() == 1)
                    {
                        return *descent(statement.parameters[0]).value() * *std::make_unique<Integer>(-1);
                    }

                    return *descent(statement.parameters[0]).value() 
                        - *descent(statement.parameters[1]).value();
                }
                else if (statement.name == "*")
                {
                    return *descent(statement.parameters[0]).value() 
                        * *descent(statement.parameters[1]).value();
                }
                else if (statement.name == "%")
                {
                    return *descent(statement.parameters[0]).value()
                        % *descent(statement.parameters[1]).value();
                }
                else if (statement.name == "&&")
                {
                    return *descent(statement.parameters[0]).value()
                        && *descent(statement.parameters[1]).value();
                }
                else if (statement.name == "||")
                {
                    return *descent(statement.parameters[0]).value()
                        || *descent(statement.parameters[1]).value();
                }
                break;
            case VARIABLE_DECLARATION: {
                if (variables_in_current_scope.find(statement.name) == variables_in_current_scope.end())
                {  
                    if (statement.type.type == INTEGER)
                    {
                        variables_in_current_scope[statement.name] = std::make_unique<Integer>(0);
                        *variables_in_current_scope[statement.name] = *descent(statement.parameters.front()).value();
                    }
                    else if (statement.type.type == STRING)
                    {
                        variables_in_current_scope[statement.name] = std::make_unique<String>("");
                        *variables_in_current_scope[statement.name] = *descent(statement.parameters.front()).value();
                    }
                    else if (statement.type.type == BOOLEAN)
                    {
                        variables_in_current_scope[statement.name] = std::make_unique<Boolean>(false);
                        *variables_in_current_scope[statement.name] = *descent(statement.parameters.front()).value();
                    }
                    else if (statement.type.type == VECTOR)
                    {
                        variables_in_current_scope[statement.name] = std::make_unique<Vector>();
                        *variables_in_current_scope[statement.name] = *descent(statement.parameters.front()).value();
                    }
                }
                else 
                {
                    throw runtime_error("\nInterpreter Error: Variable '" + statement.name + "' already defined.");
                }  
                break;
            }
            case ASSIGNMENT_EXPRESSION:
                if (variables_in_current_scope.find(statement.name) == variables_in_current_scope.end())
                {
                    throw runtime_error("\nInterpreter Error: Variable '" + statement.name + "' not defined.");
                }

                if (statement.parameters.size() == 1)
                {
                    *variables_in_current_scope[statement.name] = *descent(statement.parameters.front()).value();
                    return std::nullopt;
                }

                *variables_in_current_scope[statement.name]->operator[](*descent(statement.parameters[0]).value())
                = *descent(statement.parameters[1]).value();

                break;
            case ASSIGNMENT_PLUS_EXPRESSION:
            {
                if (variables_in_current_scope.find(statement.name) == variables_in_current_scope.end())
                {
                    throw runtime_error("\nInterpreter Error: Variable '" + statement.name + "' not defined.");
                }

                *variables_in_current_scope[statement.name] += *descent(statement.parameters.front()).value();
                break;
            }
            case ASSIGNMENT_DIVIDE_EXPRESSION:
            case ASSIGNMENT_MULTIPLY_EXPRESSION:
            case ASSIGNMENT_MINUS_EXPRESSION:
                std::cerr << "\nASSIGNMENT EXPRESSION NOT IMPLEMENTED YET" << std::endl;
                break;
            case FUNCTION_CALL:
                if (functions.find(statement.name) == functions.end())
                {
                    throw runtime_error("\nInterpreter Error: Function '" + statement.name + "' not defined.");
                }
                else
                {
                    auto potential_return_value = Parse(statement.parameters, statement.name);
                    if (potential_return_value.has_value())
                    {
                        return potential_return_value.value();
                    }
                }

                return std::nullopt;
                break;
            case RETURN_STATEMENT:
                return descent(statement.parameters.front()).value();
                break;
            default:
                std::cerr << "\n" << statement_kind_strings[statement.kind] << " kind has not yet been programmed in." << std::endl;    
        }

        return std::nullopt;
    }

    void Interpreter::clear_scope()
    {
        if (scope_start.size())
        {
            variables_in_current_scope.erase(
                variables_in_current_scope.begin(),
                variables_in_current_scope.find(scope_start.back()));

            scope_start.pop_back();
        } 
        else if (variables_in_current_scope.size())
        {
            variables_in_current_scope.erase(
                variables_in_current_scope.begin(),
                variables_in_current_scope.end());
        }
    }
}