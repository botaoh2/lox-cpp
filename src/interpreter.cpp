#include "environment.hpp"
#include "pch.hpp"

#include "error.hpp"
#include "interpreter.hpp"
#include "token.hpp"
#include "value.hpp"

class Finally
{
public:
    Finally(std::function<void()> func) : m_func(func) {}
    ~Finally() { m_func(); } // Automatically executes when going out of scope

private:
    std::function<void()> m_func;
};

void Interpreter::interpret(const Stmt& stmt)
{
    exec(stmt);
}

Value Interpreter::interpret(const Expr& expr)
{
    return eval(expr);
}

void Interpreter::exec(const Stmt& stmt)
{
    if (const auto* printStmt = dynamic_cast<const Stmt::Print*>(&stmt))
        return exec(*printStmt);
    if (const auto* exprStmt = dynamic_cast<const Stmt::Expression*>(&stmt))
        return exec(*exprStmt);
    if (const auto* varStmt = dynamic_cast<const Stmt::Var*>(&stmt))
        return exec(*varStmt);
    if (const auto* blockStmt = dynamic_cast<const Stmt::Block*>(&stmt))
        return exec(*blockStmt);
    if (const auto* ifStmt = dynamic_cast<const Stmt::If*>(&stmt))
        return exec(*ifStmt);

    assert(0 && "unreachable");
}

void Interpreter::exec(const Stmt::Print& stmt)
{
    const Value value = eval(*stmt.expression);
    fmt::println("{}", value);
}

void Interpreter::exec(const Stmt::Expression& stmt)
{
    eval(*stmt.expression);
}

void Interpreter::exec(const Stmt::Var& stmt)
{
    Value value;
    if (stmt.expression)
        value = eval(*stmt.expression);
    m_environment->define(stmt.name.lexeme, value);
}

void Interpreter::exec(const Stmt::Block& stmt)
{
    executeBlock(stmt.statements, std::make_shared<Environment>(m_environment));
}

void Interpreter::exec(const Stmt::If& stmt)
{
    auto condition = eval(*stmt.condition);
    if (isTruthy(condition))
        exec(*stmt.ifBranch);
    else if (stmt.elseBranch)
        exec(*stmt.elseBranch);
}

void Interpreter::executeBlock(const std::vector<std::unique_ptr<Stmt>>& statements, std::shared_ptr<Environment> env)
{
    auto previous = m_environment;
    Finally cleanup([&]() { m_environment = previous; });

    m_environment = env;
    for (const auto& stmt : statements)
    {
        exec(*stmt);
    }
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
    if (const auto* variableExpr = dynamic_cast<const Expr::Variable*>(&expr))
        return eval(*variableExpr);
    if (const auto* assignExpr = dynamic_cast<const Expr::Assign*>(&expr))
        return eval(*assignExpr);
    if (const auto* logicalExpr = dynamic_cast<const Expr::Logical*>(&expr))
        return eval(*logicalExpr);

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

        throw RuntimeError(expr.op, "Operands must be two numbers or two strings");
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

Value Interpreter::eval(const Expr::Variable& expr)
{
    return m_environment->get(expr.name);
}

Value Interpreter::eval(const Expr::Assign& expr)
{
    auto value = eval(*expr.value);
    m_environment->assign(expr.name, value);
    return value;
}

Value Interpreter::eval(const Expr::Logical& expr)
{
    Value left = eval(*expr.left);
    if (expr.op.type == TokenType::And)
    {
        if (!isTruthy(left))
            return left;
        return eval(*expr.right);
    }
    else if (expr.op.type == TokenType::Or)
    {
        if (isTruthy(left))
            return left;
        return eval(*expr.right);
    }
    else
    {
        assert(0 && "unreachable");
    }
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
        throw RuntimeError(token, "Operand must be a number.");
}

void Interpreter::checkNumber(const Token& token, const Value& left, const Value& right)
{
    if (!left.isNumber() || !right.isNumber())
        throw RuntimeError(token, "Operands must be numbers.");
}

void Interpreter::checkString(const Token& token, const Value& value)
{
    if (!value.isString())
        throw RuntimeError(token, "Operand must be a string.");
}
