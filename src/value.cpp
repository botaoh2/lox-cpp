#include "error.hpp"
#include "pch.hpp"

#include "environment.hpp"
#include "interpreter.hpp"
#include "value.hpp"

LoxFunction::LoxFunction(const Stmt::Fun& declaration, std::shared_ptr<Environment> closure)
    : declaration(declaration), closure(closure)
{
}

Value LoxFunction::call(Interpreter& interpreter, const std::vector<Value>& arguments)
{
    auto env = std::make_shared<Environment>(closure);

    for (int i = 0; i < arguments.size(); i++)
    {
        env->define(declaration.params[i].lexeme, arguments[i]);
    }

    try
    {
        interpreter.executeBlock(declaration.body, env);
    }
    catch (const Return& returnValue)
    {
        return returnValue.value;
    }

    return Value();
}

int LoxClass::arity() const
{
    return 0;
}

Value LoxClass::call(Interpreter& interpreter, const std::vector<Value>& arguments)
{
    return Value(std::make_shared<LoxInstance>(*this));
}

std::string LoxInstance::toString() const
{
    return fmt::format("<{} instance>", clazz.name);
}

Value LoxInstance::get(const Token& name) const
{
    if (auto it = fields.find(name.lexeme); it != fields.end())
        return it->second;

    throw RuntimeError(name, fmt::format("Undefined property '{}'", name.lexeme));
}

void LoxInstance::set(const Token& name, const Value& value)
{
    fields[name.lexeme] = value;
}

std::string format_as(const Value& value)
{
    if (value.isNil())
        return "nil";
    if (value.isBoolean())
        return value.getBoolean() ? "true" : "false";
    if (value.isNumber())
        return fmt::format("{}", value.getNumber());
    if (value.isString())
        return value.getString();
    if (value.isCallable())
        return value.getCallable()->toString();
    if (value.isInstance())
        return value.getInstance()->toString();

    assert(0 && "unreachable");
    return "";
}
