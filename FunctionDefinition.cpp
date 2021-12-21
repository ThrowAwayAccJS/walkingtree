#include "FunctionDefinition.h"
#include <iostream>

namespace parser
{
    using namespace std;

    void FunctionDefinition::debug_print() const
    {
        cout << type_string_definitions[type] << " " << name << " (" << endl;
        for (auto param : parameters)
        {
            cout << "\t" << param.type.name << " " << param.name << endl;
        }

        cout << ")\n";
        cout << "{\n";
        for (auto statement : statements)
        {
            statement.debug_print(1);
        }

        cout << "}\n" << endl;
    }
}