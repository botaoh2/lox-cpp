#include "pch.hpp"

#include "interpreter.hpp"
#include "value.hpp"

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
