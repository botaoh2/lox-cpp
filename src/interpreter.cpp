#include "pch.hpp"

#include "interpreter.hpp"
#include "token.hpp"
#include "value.hpp"

void Interpreter::interpret(const Expr& expr)
{
    auto value = eval(expr);
    fmt::println("{}", value);
}

Value Interpreter::eval(const Expr& expr)
{
    if (const auto* binaryExpr = dynamic_cast<const Expr::Binary*>(&expr))
        return eval(*binaryExpr);
    if (const auto* groupingExpr = dynamic_cast<const Expr::Grouping*>(&expr))
        return eval(*groupingExpr);
    if (const auto* literalExpr = dynamic_cast<const Expr::Literal*>(&expr))
        return eval(*literalExpr);
    if (const auto* unaryExpr = dynamic_cast<const Expr::Unary*>(&expr))
        return eval(*unaryExpr);

    assert(0 && "unreachable");
    return Value();
}

Value Interpreter::eval(const Expr::Binary& expr)
{
    auto leftValue = eval(*expr.left);
    auto rightValue = eval(*expr.right);

    if (expr.op.type == TokenType::Plus)
    {
        if (leftValue.isNumber() && rightValue.isNumber())
            return Value(leftValue.getNumber() + rightValue.getNumber());
        if (leftValue.isString() && rightValue.isString())
            return Value(leftValue.getString() + rightValue.getString());

        throw Error(expr.op, "Operands must be two numbers or two strings");
    }

    if (expr.op.type == TokenType::Less)
    {
        checkNumber(expr.op, leftValue, rightValue);
        return Value(leftValue.getNumber() < rightValue.getNumber());
    }

    if (expr.op.type == TokenType::LessEqual)
    {
        checkNumber(expr.op, leftValue, rightValue);
        return Value(leftValue.getNumber() <= rightValue.getNumber());
    }

    if (expr.op.type == TokenType::Greater)
    {
        checkNumber(expr.op, leftValue, rightValue);
        return Value(leftValue.getNumber() > rightValue.getNumber());
    }

    if (expr.op.type == TokenType::GreaterEqual)
    {
        checkNumber(expr.op, leftValue, rightValue);
        return Value(leftValue.getNumber() >= rightValue.getNumber());
    }

    if (expr.op.type == TokenType::Minus)
    {
        checkNumber(expr.op, leftValue, rightValue);
        return Value(leftValue.getNumber() - rightValue.getNumber());
    }

    if (expr.op.type == TokenType::Slash)
    {
        checkNumber(expr.op, leftValue, rightValue);
        return Value(leftValue.getNumber() / rightValue.getNumber());
    }

    if (expr.op.type == TokenType::Star)
    {
        checkNumber(expr.op, leftValue, rightValue);
        return Value(leftValue.getNumber() * rightValue.getNumber());
    }

    assert(0 && "unreachable");
    return Value();
}

Value Interpreter::eval(const Expr::Grouping& expr)
{
    return eval(*expr.expression);
}

Value Interpreter::eval(const Expr::Literal& expr)
{
    if (expr.value.type == TokenType::Nil)
        return Value();
    if (expr.value.type == TokenType::False)
        return Value(false);
    if (expr.value.type == TokenType::True)
        return Value(true);
    if (expr.value.type == TokenType::Number)
        return Value(std::stod(std::string(expr.value.lexeme)));
    if (expr.value.type == TokenType::String)
        return Value(std::string(expr.value.lexeme.substr(1, expr.value.lexeme.size() - 2)));

    assert(0 && "unreachable");
    return Value();
}

Value Interpreter::eval(const Expr::Unary& expr)
{
    if (expr.op.type == TokenType::Minus)
    {
        auto value = eval(*expr.right);
        checkNumber(expr.op, value);
        return Value(-value.getNumber());
    }
    if (expr.op.type == TokenType::Bang)
    {
        auto value = eval(*expr.right);
        return Value(!isTruthy(value));
    }

    assert(0 && "unreachable");
    return Value();
}

bool Interpreter::isTruthy(const Value& value)
{
    if (value.isNil())
        return false;
    if (value.isBoolean())
        return value.getBoolean();
    return true;
}

void Interpreter::checkNumber(const Token& token, const Value& value)
{
    if (!value.isNumber())
        throw Error(token, "Operand must be a number.");
}

void Interpreter::checkNumber(const Token& token, const Value& left, const Value& right)
{
    if (!left.isNumber() || !right.isNumber())
        throw Error(token, "Operands must be numbers.");
}

void Interpreter::checkString(const Token& token, const Value& value)
{
    if (!value.isString())
        throw Error(token, "Operand must be a string.");
}
