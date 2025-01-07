#include "environment.hpp"

#include "error.hpp"

const Value& Environment::get(const Token& name) const
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

const Value& Environment::getAt(int distance, const std::string& name) const
{
    return ancestor(distance).m_values.at(name);
}

void Environment::assignAt(int distance, const std::string& name, const Value& value)
{
    ancestor(distance).m_values.at(name) = value;
}

Environment& Environment::ancestor(int distance)
{
    assert(distance >= 0);

    Environment* env = this;
    for (int i = 0; i < distance; i++)
        env = env->m_parent.get();

    return *env;
}

const Environment& Environment::ancestor(int distance) const
{
    assert(distance >= 0);

    const Environment* env = this;
    for (int i = 0; i < distance; i++)
        env = env->m_parent.get();

    return *env;
}
