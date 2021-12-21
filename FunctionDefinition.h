#pragma once

#include <string>
#include "Type.h"
#include "Statement.h"

namespace parser
{
    using namespace std;

    class ParameterDefinition
    {
    public:
        string name;
        Type type;
    };

    class FunctionDefinition
    {
    public:
        string name;
        TYPE type;
        vector<ParameterDefinition> parameters;
        vector<Statement> statements;
        bool external = false;

        void debug_print() const;
    };
}