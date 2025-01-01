#pragma once

#include "token.hpp"
#include "value.hpp"

#include <unordered_map>

class Environment
{
public:
    bool contains(const std::string& name) const;
    Value get(const Token& name) const;

    void define(const std::string& name, const Value& value);
    void assign(const Token& name, const Value& value);

private:
    std::unordered_map<std::string, Value> m_values;
};
