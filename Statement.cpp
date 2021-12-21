#include "Statement.h"
#include <iostream>

namespace parser
{
    using namespace std;

    void Statement::debug_print(size_t indent)
    {
        cout << string(indent, '\t') << statement_kind_strings[kind] << " " << type.name << " " << name << " (" << endl;
        for (auto param : parameters)
        {
            param.debug_print(indent + 1);
        }

        cout << string(indent, '\t') << ")" << endl;
    }
}