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

bool Environment::define(const std::string& name, const Value& value)
{
    if (m_values.contains(name))
        return false;

    m_values[name] = value;
    return true;
}
