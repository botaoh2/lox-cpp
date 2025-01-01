#pragma once

#include "expr.hpp"

class Stmt
{
public:
    class Expression;
    class Print;

    virtual ~Stmt() = default;
};

class Stmt::Expression : public Stmt
{
public:
    Expression(std::unique_ptr<Expr> expression) : expression(std::move(expression)) {}

    std::unique_ptr<Expr> expression;
};

class Stmt::Print : public Stmt
{
public:
    Print(std::unique_ptr<Expr> expression) : expression(std::move(expression)) {}

    std::unique_ptr<Expr> expression;
};
