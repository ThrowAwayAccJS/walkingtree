#include "Any.h"
#include <iostream>

// Integer Type overrides

namespace parser
{
/*     std::shared_ptr<VariableType> VariableType::property_access(VariableType& other) override
    {
        throw runtime_error("Expression is not callable '" + type_to_string[this->GetType()] + "' has no '" + other.GetValue() + "' property.");
    } */

    Integer& Integer::operator+=(VariableType& other)
    {
        this->value += std::stoi(other.GetValue());
        return *this;
    }

    Integer& Integer::operator=(VariableType& other) 
    {
        this->value = std::stoi(other.GetValue());
        return *this;
    }

    std::shared_ptr<VariableType> Integer::operator+(VariableType& other)
    {
        switch (other.GetType())
        {
            case INTEGER:
                return std::make_unique<Integer>(this->value + std::stoi(other.GetValue()));
                break;
            case STRING:
                return std::make_unique<String>(this->GetValue() + other.GetValue());
                break;
            case BOOLEAN:
                return std::make_unique<Boolean>(this->value + (other.GetValue() == "true" ? 1 : 0));
                break;
            default:
                std::cerr << "UNKNOWN OTHER TYPE!" << std::endl;
                return nullptr;
        }
    }

    std::shared_ptr<VariableType> Integer::operator-(VariableType& other)
    {
        switch (other.GetType())
        {
            case INTEGER:
                return std::make_unique<Integer>(this->value - std::stoi(other.GetValue()));
                break;
            case STRING:
                return std::make_unique<String>(this->GetValue() + other.GetValue());
                break;
            case BOOLEAN:
                return std::make_unique<Boolean>(this->value - (other.GetValue() == "true" ? 1 : 0));
                break;
            default:
                std::cerr << "UNKNOWN OTHER TYPE!" << std::endl;
                return nullptr;
        }
    }

    std::shared_ptr<VariableType> Integer::operator*(VariableType& other)
    {
        switch (other.GetType())
        {
            case INTEGER:
                return std::make_unique<Integer>(this->value * std::stoi(other.GetValue()));
                break;
            case STRING:
                throw std::runtime_error("Cannot multiply integer by string!");
            case BOOLEAN:
                return std::make_unique<Boolean>(this->value * (other.GetValue() == "true" ? 1 : 0));
                break;
            default:
                std::cerr << "UNKNOWN OTHER TYPE!" << std::endl;
                return nullptr;
        }
    }

    std::shared_ptr<VariableType> Integer::operator[](VariableType& other)
    {
        throw runtime_error("Cannot index a type of 'Integer'");
    }

    bool Integer::operator==(VariableType& other)
    {
        if (other.GetType() != INTEGER)
        {
            return false;
        }

        return this->GetValue() == other.GetValue();
    }

    bool Integer::operator<(VariableType& other)
    {
        if (other.GetType() == INTEGER)
        {
            return this->value < std::stoi(other.GetValue());
        }

        // implement some error message saying  you cant do that or smth
        return false;
    }

    std::shared_ptr<VariableType> Integer::operator&&(VariableType& other)
    {
        if (this->value > 0)
        {
            switch (other.GetType())
            {
                case INTEGER:
                    return std::make_shared<Boolean>(std::stoi(other.GetValue()));
                    break;
                case STRING:
                    return std::make_shared<Boolean>(other.GetValue().size() > 0);
                    break;
                case BOOLEAN:
                    return std::make_shared<Boolean>(other.GetValue() == "true" ? true : false);
                    break;
            }
        }

        return std::make_shared<Boolean>(false);
    }

    std::shared_ptr<VariableType> Integer::operator||(VariableType& other)
    {
        switch (other.GetType())
        {
            case INTEGER:
                return std::make_shared<Boolean>(this->value > 0 || std::stoi(other.GetValue()));
                break;
            case STRING:
                return std::make_shared<Boolean>(this->value > 0 || other.GetValue().size() > 0);
                break;
            case BOOLEAN:
                return std::make_shared<Boolean>(this->value > 0 || other.GetValue() == "true" ? true : false);
                break;
            default:
                std::cerr << "UNKNOWN OTHER TYPE!" << std::endl;
                return nullptr;
        }
    }

    std::shared_ptr<VariableType> Integer::operator%(VariableType& other)
    {
        if (other.GetType() == INTEGER)
        {
            return std::make_shared<Integer>(this->value % std::stoi(other.GetValue()));
        }

        throw std::runtime_error("Cannot modulo integer by non-integer!");
    }

    Integer::Integer(const int num)
    {
        this->value = num;
    }

    std::string Integer::GetValue()
    {
        return std::to_string(this->value);
    }

    Integer::operator bool()
    {
        return this->value > 0;
    }

    std::shared_ptr<VariableType> Integer::property_access(std::string property)
    {
        throw runtime_error("Expression is not callable '" + type_to_string[this->GetType()] + "' has no '" + property + "' property.");
    }

    TYPE Integer::GetType()
    {
        return INTEGER;
    }

/*     Integer::~Integer()
    {
        std::cout << "Integer destructor called" << std::endl;
    } */

    // end

    // string overrides

    std::string String::GetValue()
    {
        return value;
    }

    String::String(std::string str)
    {
        this->value = str;
    }

    String& String::operator+=(VariableType& other)
    {
        this->value += other.GetValue();
        return *this;
    }

    String& String::operator=(VariableType& other)
    {
        this->value = other.GetValue();
        return *this;
    }

    std::shared_ptr<VariableType> String::operator+(VariableType& other)
    {
        switch (other.GetType())
        {
            case STRING:
                return std::make_shared<String>(this->GetValue() + other.GetValue());
            default: 
                throw runtime_error("No known conversion from '" + type_to_string[BOOLEAN] + "' to string");
        }
    }

    std::shared_ptr<VariableType> String::operator-(VariableType& other)
    {
        throw std::runtime_error("Cannot subtract from a string");
    }

    std::shared_ptr<VariableType> String::property_access(std::string property)
    {
        if (property == "length")
        {
            return std::make_shared<Integer>(this->value.size());
        }

        throw runtime_error("Expression is not callable '" + type_to_string[this->GetType()] + "' has no '" + property + "' property.");
    }

    std::shared_ptr<VariableType> String::operator*(VariableType& other)
    {
        throw std::runtime_error("Cannot multiply a string");
    }

    std::shared_ptr<VariableType> String::operator%(VariableType& other)
    {
        throw std::runtime_error("Cannot modulo non-integers!");
    }

    std::shared_ptr<VariableType> String::operator[](VariableType& other)
    {
        if (other.GetType() != INTEGER)
        {
            throw runtime_error("Cannot index a string using a '" + type_to_string[other.GetType()] + "'");
        }

        const char val = this->value[std::stoi(other.GetValue())];
        return std::make_shared<String>(std::string(1, val));
    }

    bool String::operator==(VariableType& other)
    {
        if (other.GetType() != STRING)
        {
            return false;   
        }
        return this->value == other.GetValue();
    }

    bool String::operator<(VariableType& other)
    {
        if (other.GetType() == STRING)
        {
            return this->value.length() < other.GetValue().length();
        }

        return false;
    }

    std::shared_ptr<VariableType> String::operator&&(VariableType& other)
    {
        if (this->value.size() > 0)
        {
            switch (other.GetType())
            {
                case INTEGER:
                    return std::make_shared<Boolean>(std::stoi(other.GetValue()));
                    break;
                case STRING:
                    return std::make_shared<Boolean>(other.GetValue().size() > 0);
                    break;
                case BOOLEAN:
                    return std::make_shared<Boolean>(other.GetValue() == "true" ? true : false);
                    break;
            }
        }

        return std::make_shared<Boolean>(false);
    }

    std::shared_ptr<VariableType> String::operator||(VariableType& other)
    {
        switch (other.GetType())
        {
            case INTEGER:
                return std::make_shared<Boolean>(this->value.size() > 0 || std::stoi(other.GetValue()));
                break;
            case STRING:
                return std::make_shared<Boolean>(this->value.size() > 0 || other.GetValue().size() > 0);
                break;
            case BOOLEAN:
                return std::make_shared<Boolean>(this->value.size() > 0 || other.GetValue() == "true" ? true : false);
                break;
            default:
                throw runtime_error("No known conversion from '" + type_to_string[other.GetType()] + "' to string");
        }
    }

    String::operator bool()
    {
        return this->value.length() > 0;
    }

    TYPE String::GetType()
    {
        return STRING;
    }

/*     String::~String()
    {
        std::cout << "String destructor called" << std::endl;
    } */

    // end

    // boolean

    Boolean::Boolean(bool initial)
    {
        this->value = initial;
    }

    Boolean& Boolean::operator+=(VariableType& other)
    {
        return *this;
    }

    Boolean& Boolean::operator=(VariableType& other)
    {
        switch(other.GetType())
        {
            case INTEGER:
                break;
            case STRING:
                break;
            case BOOLEAN:
                this->value = other.GetValue() == "true" ? true : false;
                return *this;
        }

        std::cerr << "UNKNOWN TYPE!" << std::endl;
        return *this;
    }

    std::shared_ptr<VariableType> Boolean::operator+(VariableType& other)
    {
        switch (other.GetType())
        {
            case INTEGER:
                return std::make_unique<Integer>(this->value + std::stoi(other.GetValue()));
                break;
            case STRING:
                return std::make_unique<String>(this->GetValue() + other.GetValue());
                break;
            case BOOLEAN:
                return std::make_unique<Boolean>(this->value + (other.GetValue() == "true" ? 1 : 0));
                break;
            default:
                std::cerr << "UNKNOWN OTHER TYPE!" << std::endl;
                return nullptr;
        }
    }

    std::shared_ptr<VariableType> Boolean::operator-(VariableType& other)
    {
        switch (other.GetType())
        {
            case INTEGER:
                return std::make_unique<Integer>(this->value - std::stoi(other.GetValue()));
                break;
            case STRING:
                throw std::runtime_error("Cannot subtract from a string");
                break;
            case BOOLEAN:
                return std::make_unique<Boolean>(this->value - (other.GetValue() == "true" ? 1 : 0));
                break;
            default:
                std::cerr << "UNKNOWN OTHER TYPE!" << std::endl;
                return nullptr;
        }
    }

    std::shared_ptr<VariableType> Boolean::operator*(VariableType& other)
    {
        throw std::runtime_error("Cannot multiply booleans");
    }

    std::shared_ptr<VariableType> Boolean::operator%(VariableType& other)
    {
        throw std::runtime_error("Cannot modulo non-integers!");
    }

    std::shared_ptr<VariableType> Boolean::operator[](VariableType& other)
    {
        throw std::runtime_error("Cannot index a boolean.");
    }

    bool Boolean::operator==(VariableType& other)
    {
        return this->GetValue() == other.GetValue();
    }

    bool Boolean::operator<(VariableType& other)
    {
        return this->value;
    }

    std::shared_ptr<VariableType> Boolean::operator&&(VariableType& other)
    {
        if (this->value)
        {
            switch (other.GetType())
            {
                case INTEGER:
                    return std::make_shared<Boolean>(std::stoi(other.GetValue()));
                    break;
                case STRING:
                    return std::make_shared<Boolean>(other.GetValue().size() > 0);
                    break;
                case BOOLEAN:
                    return std::make_shared<Boolean>(other.GetValue() == "true" ? true : false);
                    break;
            }
        }

        return std::make_shared<Boolean>(false);
    }

    std::shared_ptr<VariableType> Boolean::operator||(VariableType& other)
    {
        switch (other.GetType())
        {
            case INTEGER:
                return std::make_shared<Boolean>(this->value || std::stoi(other.GetValue()));
                break;
            case STRING:
                return std::make_shared<Boolean>(this->value || other.GetValue().size() > 0);
                break;
            case BOOLEAN:
                return std::make_shared<Boolean>(this->value || other.GetValue() == "true" ? true : false);
                break;
            default:
                throw runtime_error("No known conversion from '" + type_to_string[other.GetType()] + "' to string");
        }
    }

    std::string Boolean::GetValue()
    {
        return this->value == true ? "true" : "false";
    }

    Boolean::operator bool()
    {
        return this->value;
    }

    std::shared_ptr<VariableType> Boolean::property_access(std::string property)
    {
        throw runtime_error("Expression is not callable '" + type_to_string[this->GetType()] + "' has no '" + property + "' property.");
    }

    TYPE Boolean::GetType()
    {
        return BOOLEAN;
    }

/*     Boolean::~Boolean()
    {
        std::cout << "Boolean destructor called" << std::endl;
    } */

    // end

    // vector 


    Vector::Vector(std::vector<std::shared_ptr<VariableType>> initial)
    {
        for (auto& item : initial)
        {
            this->value.push_back(item);
        }
    }

    Vector& Vector::operator+=(VariableType& other)
    {
        throw std::runtime_error("Cannot add to a vector yet.");
    }

    Vector& Vector::operator=(VariableType& other)
    {
        if (other.GetType() == VECTOR)
        {
            this->value.erase(this->value.begin(), this->value.end());
            for (auto x = std::make_shared<Integer>(0); 
                *x < *other.property_access("size");
                *x += *std::make_unique<Integer>(1))
            {
                this->value.push_back(other.operator[](*x));
            }

            return *this;
        }

        throw std::runtime_error("Cannot assign a non-vector to a vector.");
    }

    std::shared_ptr<VariableType> Vector::operator+(VariableType& other)
    {
        throw std::runtime_error("Cannot add to a vector yet.");
    }

    std::shared_ptr<VariableType> Vector::operator-(VariableType& other)
    {
        throw std::runtime_error("Cannot subtract from a vector yet.");
    }

    std::shared_ptr<VariableType> Vector::operator*(VariableType& other)
    {
        throw std::runtime_error("Cannot times a vector.");
    }

    std::shared_ptr<VariableType> Vector::operator[](VariableType& other)
    {
        if (other.GetType() == INTEGER)
        {
            int index = std::stoi(other.GetValue());
            if (index < 0)
            {
                throw std::runtime_error("Index cannot be negative.");
            }

            if (index >= this->value.size())
            {
                throw std::runtime_error("Index out of bounds.");
            }

            return this->value[index];
        }

        throw std::runtime_error("Cannot index a vector with a non-integer.");
    }

    bool Vector::operator==(VariableType& other)
    {
        return false;
    }

    bool Vector::operator<(VariableType& other)
    {
        return false;
    }

    std::shared_ptr<VariableType> Vector::operator&&(VariableType& other)
    {
        return std::make_shared<Boolean>(false);
    }

    std::shared_ptr<VariableType> Vector::operator||(VariableType& other)
    {
        return std::make_shared<Boolean>(false);
    }

    std::shared_ptr<VariableType> Vector::operator%(VariableType& other)
    {
        throw std::runtime_error("Cannot modulo a vector.");
    }

    std::string Vector::GetValue()
    {
        std::stringstream ss;
        ss << "[";
        for (int i = 0; i < this->value.size(); i++)
        {
            ss << this->value[i]->GetValue();
            if (i != this->value.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "]";
        return ss.str();
    }

    Vector::operator bool()
    {
        return this->value.size() > 0;
    }

    std::shared_ptr<VariableType> Vector::property_access(std::string property)
    {
        if (property == "size")
        {
            return std::make_shared<Integer>(this->value.size());
        }

        throw std::runtime_error("Not implemeted yet");
    }

    TYPE Vector::GetType()
    {
        return VECTOR;
    }
}