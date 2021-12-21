#pragma once 

#include "FunctionDefinition.h"
#include "Any.h"
#include <unordered_map>
#include <optional>
#include <iterator>

namespace parser
{
    typedef std::optional<std::shared_ptr<VariableType>> (*ExternalFunction)(unordered_map<std::string, std::shared_ptr<VariableType>>&);

    std::optional<std::shared_ptr<VariableType>> cout_println(unordered_map<std::string, std::shared_ptr<VariableType>> &variables_in_current_scope);
    std::optional<std::shared_ptr<VariableType>> cout_print(unordered_map<std::string, std::shared_ptr<VariableType>> &variables_in_current_scope);
    std::optional<std::shared_ptr<VariableType>> cin_read(unordered_map<std::string, std::shared_ptr<VariableType>> &variables_in_current_scope);

    class Interpreter
    {
        public:
            Interpreter(unordered_map<std::string, FunctionDefinition> &functions);
            std::optional<std::shared_ptr<VariableType>> Parse(std::vector<Statement> args = {}, std::string entry_point = "main");
        private:
            unordered_map<std::string, FunctionDefinition> functions;
            unordered_map<std::string, std::shared_ptr<VariableType>> variables_in_current_scope;

            std::optional<std::shared_ptr<VariableType>> descent(Statement &statement);

            std::vector<std::string> scope_start;
            std::unordered_map<std::string, ExternalFunction> external_functions;

            void clear_scope();
    };
}