#pragma once

#include "token.hpp"
#include "value.hpp"

#include <unordered_map>

class Environment
{
public:
    Environment() {}
    Environment(std::shared_ptr<Environment> parent) : m_parent(parent) {}

    const Value& get(const Token& name) const;

    void define(const std::string& name, const Value& value);
    void assign(const Token& name, const Value& value);

    const Value& getAt(int distance, const std::string& name) const;
    void assignAt(int distance, const std::string& name, const Value& value);

    Environment& ancestor(int distance);
    const Environment& ancestor(int distance) const;

private:
    std::shared_ptr<Environment> m_parent;
    std::unordered_map<std::string, Value> m_values;
};
