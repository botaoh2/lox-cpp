#include "environment.hpp"

#include "error.hpp"

bool Environment::contains(const std::string& name) const
{
    return m_values.contains(name);
}

Value Environment::get(const Token& name) const
{
    if (auto it = m_values.find(name.lexeme); it != m_values.end())
        return it->second;

    throw RuntimeError(name, fmt::format("Undefined variable '{}'", name.lexeme));
}

void Environment::define(const std::string& name, const Value& value)
{
    m_values[name] = value;
}

void Environment::assign(const Token& name, const Value& value)
{
    if (auto it = m_values.find(name.lexeme); it != m_values.end())
    {
        it->second = value;
        return;
    }

    throw RuntimeError(name, fmt::format("Undefined variable '{}'", name.lexeme));
}
