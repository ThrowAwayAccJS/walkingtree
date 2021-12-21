#pragma once

#include <string>
#include <vector>

namespace parser
{
    using namespace std;

    enum TYPE
    {
        INTEGER,
        DOUBLE,
        STRING,
        VOID,
        BOOLEAN,
        VECTOR,
        LENGTH,
    };

    static const char* type_string_definitions[] =
    {
        "INTEGER",
        "DOUBLE",
        "STRING",
        "VOID",
        "BOOLEAN",
        "VECTOR",
    };

    class Type
    {
    public:
        Type(const string &name = "", enum TYPE type = TYPE::VOID) : name(name), type(type) {};

        string name;
        enum TYPE type;
        vector<Type> fields;
    };
}