#include "pch.hpp"

#include "interpreter.hpp"
#include "value.hpp"

Value LoxFunction::call(Interpreter& interpreter, const std::vector<Value>& arguments)
{
    auto env = std::make_shared<Environment>(interpreter.m_global);

    for (int i = 0; i < arguments.size(); i++)
    {
        env->define(declaration.params[i].lexeme, arguments[i]);
    }

    interpreter.executeBlock(declaration.body, env);
    return Value();
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
        return "Callable";

    assert(0 && "unreachable");
    return "";
}
