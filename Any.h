#include <unordered_map>
#include <iostream>
#include <string>
#include <memory>
#include <sstream>
#include <vector>
#include "Type.h"

namespace parser
{
    static std::string type_to_string[] =
    {
        "INTEGER",
        "INTEGER_ARRAY",
        "DOUBLE",
        "DOUBLE_ARRAY",
        "STRING",
        "STRING_ARRAY",
        "VOID",
        "VOID_ARRAY",
        "BOOLEAN",
        "ARRAY",
        "BOOLEAN_ARRAY",
    };

    class VariableType
    {
    public:
        virtual TYPE GetType() = 0;
        virtual std::string GetValue() = 0;
        virtual VariableType& operator+=(VariableType& other) = 0;
        virtual VariableType& operator=(VariableType& other) = 0;
        virtual std::shared_ptr<VariableType> operator+(VariableType& other) = 0;
        virtual std::shared_ptr<VariableType> operator-(VariableType& other) = 0;
        virtual std::shared_ptr<VariableType> operator*(VariableType& other) = 0;
        virtual bool operator==(VariableType& other) = 0;
        virtual bool operator<(VariableType& other) = 0;
        virtual std::shared_ptr<VariableType> operator&&(VariableType& other) = 0;
        virtual std::shared_ptr<VariableType> operator||(VariableType& other) = 0;
        virtual std::shared_ptr<VariableType> operator%(VariableType& other) = 0;
        virtual operator bool() = 0;
        virtual std::shared_ptr<VariableType> operator[](VariableType& other) = 0;

        virtual std::shared_ptr<VariableType> property_access(std::string property) = 0;
    };

    class String : public VariableType
    {
    private:
        std::string value;
    public:
        String(std::string str);

        std::string GetValue() override;
        String& operator+=(VariableType& other) override;
        String& operator=(VariableType& other) override;
        std::shared_ptr<VariableType> operator+(VariableType& other) override;
        std::shared_ptr<VariableType> operator-(VariableType& other) override;
        std::shared_ptr<VariableType> operator*(VariableType& other) override;
        std::shared_ptr<VariableType> operator[](VariableType& other) override;
        bool operator==(VariableType& other) override;
        bool operator<(VariableType& other) override;
        std::shared_ptr<VariableType> operator&&(VariableType& other) override;
        std::shared_ptr<VariableType> operator||(VariableType& other) override;
        std::shared_ptr<VariableType> operator%(VariableType& other) override;
        operator bool() override;

        std::shared_ptr<VariableType> property_access(std::string property) override;

        TYPE GetType() override;

        //~String();
    };

    class Integer : public VariableType
    {
    private:
        int value;
    public:
        Integer(const int num);

        std::string GetValue() override;
        Integer& operator+=(VariableType& other) override;
        Integer& operator=(VariableType& other) override;
        std::shared_ptr<VariableType> operator+(VariableType& other) override;
        std::shared_ptr<VariableType> operator-(VariableType& other) override;
        std::shared_ptr<VariableType> operator*(VariableType& other) override;
        std::shared_ptr<VariableType> operator[](VariableType& other) override;
        bool operator==(VariableType& other) override;
        bool operator<(VariableType& other) override;
        std::shared_ptr<VariableType> operator&&(VariableType& other) override;
        std::shared_ptr<VariableType> operator||(VariableType& other) override;
        std::shared_ptr<VariableType> operator%(VariableType& other) override;
        operator bool() override;
        TYPE GetType() override;

        std::shared_ptr<VariableType> property_access(std::string property) override;

        void push_back(VariableType& other);

        //~Integer();
    };

    class Boolean : public VariableType
    {
    private:
        bool value;
    public:
        Boolean(bool initial);

        Boolean& operator+=(VariableType& other) override;
        Boolean& operator=(VariableType& other) override;
        std::shared_ptr<VariableType> operator+(VariableType& other) override;
        std::shared_ptr<VariableType> operator-(VariableType& other) override;
        std::shared_ptr<VariableType> operator*(VariableType& other) override;
        std::shared_ptr<VariableType> operator[](VariableType& other) override;
        bool operator==(VariableType& other) override;
        bool operator<(VariableType& other) override;
        std::shared_ptr<VariableType> operator&&(VariableType& other) override;
        std::shared_ptr<VariableType> operator||(VariableType& other) override;
        std::shared_ptr<VariableType> operator%(VariableType& other) override;
        std::string GetValue() override;
        operator bool() override;

        std::shared_ptr<VariableType> property_access(std::string property) override;

        TYPE GetType() override;

        //~Boolean();
    };

    class Vector : public VariableType
    {
    private:
        std::vector<std::shared_ptr<VariableType>> value;
    public:
        Vector(std::vector<std::shared_ptr<VariableType>> initial = {});

        Vector& operator+=(VariableType& other) override;
        Vector& operator=(VariableType& other) override;
        std::shared_ptr<VariableType> operator+(VariableType& other) override;
        std::shared_ptr<VariableType> operator-(VariableType& other) override;
        std::shared_ptr<VariableType> operator*(VariableType& other) override;
        std::shared_ptr<VariableType> operator[](VariableType& other) override;
        bool operator==(VariableType& other) override;
        bool operator<(VariableType& other) override;
        std::shared_ptr<VariableType> operator&&(VariableType& other) override;
        std::shared_ptr<VariableType> operator||(VariableType& other) override;
        std::shared_ptr<VariableType> operator%(VariableType& other) override;
        std::string GetValue() override;
        operator bool() override;

        std::shared_ptr<VariableType> property_access(std::string property) override;

        TYPE GetType() override;

        //~Vector();
    };
}