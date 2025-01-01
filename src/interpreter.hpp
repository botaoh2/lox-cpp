#pragma once

#include "environment.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "value.hpp"

class Interpreter
{
public:
    void interpret(const Stmt& stmt);
    Value interpret(const Expr& expr);

private:
    void exec(const Stmt& stmt);
    void exec(const Stmt::Print& stmt);
    void exec(const Stmt::Expression& stmt);
    void exec(const Stmt::Var& stmt);

    Value eval(const Expr& expr);
    Value eval(const Expr::Binary& expr);
    Value eval(const Expr::Grouping& expr);
    Value eval(const Expr::Literal& expr);
    Value eval(const Expr::Unary& expr);
    Value eval(const Expr::Variable& expr);

    bool isTruthy(const Value& value);
    void checkNumber(const Token& token, const Value& value);
    void checkNumber(const Token& token, const Value& left, const Value& right);
    void checkString(const Token& token, const Value& value);

    Environment m_global;
};
