#include "pch.hpp"

#include "environment.hpp"
#include "error.hpp"
#include "interpreter.hpp"
#include "token.hpp"
#include "value.hpp"

#include <chrono>

namespace
{

class Finally
{
public:
    Finally(std::function<void()> func) : m_func(func) {}
    ~Finally() { m_func(); } // Automatically executes when going out of scope

private:
    std::function<void()> m_func;
};

class NativeFunction : public ICallable
{
public:
    std::string toString() const override { return "<native func>"; }
};

class Clock : public NativeFunction
{
public:
    Value call(Interpreter& interpreter, const std::vector<Value>&) override
    {
        std::chrono::duration<double> duration = std::chrono::system_clock::now() - s_startTime;
        return Value(duration.count());
    }

    int arity() const override { return 0; }

private:
    inline static auto s_startTime = std::chrono::system_clock::now();
};

} // namespace

Interpreter::Interpreter()
{
    m_global->define("clock", Value(std::make_shared<Clock>()));
}

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
    if (const auto* whileStmt = dynamic_cast<const Stmt::While*>(&stmt))
        return exec(*whileStmt);
    if (const auto* forStmt = dynamic_cast<const Stmt::For*>(&stmt))
        return exec(*forStmt);
    if (const auto* funStmt = dynamic_cast<const Stmt::Fun*>(&stmt))
        return exec(*funStmt);

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

void Interpreter::exec(const Stmt::While& stmt)
{
    while (isTruthy(eval(*stmt.condition)))
        exec(*stmt.body);
}

void Interpreter::exec(const Stmt::For& stmt)
{
    if (stmt.initializer)
        exec(*stmt.initializer);

    while (isTruthy(eval(*stmt.condition)))
    {
        exec(*stmt.body);
        eval(*stmt.step);
    }
}

void Interpreter::exec(const Stmt::Fun& stmt)
{
    auto functionValue = Value(std::make_shared<LoxFunction>(stmt));
    m_environment->define(stmt.name.lexeme, functionValue);
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
    if (const auto* callExpr = dynamic_cast<const Expr::Call*>(&expr))
        return eval(*callExpr);

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

    if (expr.op.type == TokenType::EqualEqual)
    {
        return Value(isEqual(leftValue, rightValue));
    }

    if (expr.op.type == TokenType::BangEqual)
    {
        return Value(!isEqual(leftValue, rightValue));
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
    m_environment->assign(expr.name, std::move(value));
    return m_environment->get(expr.name);
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

Value Interpreter::eval(const Expr::Call& expr)
{
    auto callee = eval(*expr.callee);

    std::vector<Value> arguments;
    for (const auto& arg : expr.arguments)
        arguments.emplace_back(eval(*arg));

    if (!callee.isCallable())
        throw RuntimeError(expr.paren, "Value is not callable");

    const int arity = callee.getCallable()->arity();
    if (arity != arguments.size())
    {
        throw RuntimeError(expr.paren, fmt::format("Expected {} arguments but got {}.", arity, arguments.size()));
    }

    return callee.getCallable()->call(*this, arguments);
}

bool Interpreter::isTruthy(const Value& value)
{
    if (value.isNil())
        return false;
    if (value.isBoolean())
        return value.getBoolean();
    return true;
}

bool Interpreter::isEqual(const Value& left, const Value& right)
{
    if (left.getType() != right.getType())
        return false;
    if (left.isNil())
        return true;
    if (left.isBoolean())
        return left.getBoolean() == right.getBoolean();
    if (left.isNumber())
        return left.getNumber() == right.getNumber();
    if (left.isString())
        return left.getString() == right.getString();

    assert(0 && "unreachable");
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
