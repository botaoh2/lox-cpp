#include "environment.hpp"

#include "error.hpp"

Value Environment::get(const Token& name) const
{
    if (auto it = m_values.find(name.lexeme); it != m_values.end())
        return it->second;

    if (m_parent)
        return m_parent->get(name);

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

    if (m_parent)
    {
        m_parent->assign(name, value);
        return;
    }

    throw RuntimeError(name, fmt::format("Undefined variable '{}'", name.lexeme));
}
