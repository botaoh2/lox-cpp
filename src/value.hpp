#pragma once

#include <variant>

class Value
{
public:
    Value() : m_variant(Nil{}) {}
    Value(bool value) : m_variant(Boolean{value}) {}
    Value(double value) : m_variant(Number{value}) {}
    Value(const std::string& value) : m_variant(String{value}) {}

    bool isNil() const { return std::holds_alternative<Nil>(m_variant); }
    bool isBoolean() const { return std::holds_alternative<Boolean>(m_variant); }
    bool isNumber() const { return std::holds_alternative<Number>(m_variant); }
    bool isString() const { return std::holds_alternative<String>(m_variant); }

    void setNil() { m_variant = Nil{}; }
    void setBoolean(bool value) { m_variant = Boolean{value}; }
    void setNumber(double value) { m_variant = Number{value}; }
    void setString(const std::string& value) { m_variant = String{value}; }

    bool getBoolean() const { return std::get<Boolean>(m_variant).value; }
    double getNumber() const { return std::get<Number>(m_variant).value; }
    std::string getString() const { return std::get<String>(m_variant).value; }

private:
    struct Nil
    {
    };
    struct Boolean
    {
        bool value;
    };
    struct Number
    {
        double value;
    };
    struct String
    {
        std::string value;
    };

    std::variant<Nil, Boolean, Number, String> m_variant = Nil{};
};

std::string format_as(const Value& value);
