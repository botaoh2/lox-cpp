#pragma once

#include "expr.hpp"

class Stmt
{
public:
    class Expression;
    class Print;
    class Var;

    virtual ~Stmt() = default;
};

class Stmt::Expression : public Stmt
{
public:
    Expression(std::unique_ptr<Expr> expression) : expression(std::move(expression)) { assert(this->expression); }

    std::unique_ptr<Expr> expression;
};

class Stmt::Print : public Stmt
{
public:
    Print(std::unique_ptr<Expr> expression) : expression(std::move(expression)) { assert(this->expression); }

    std::unique_ptr<Expr> expression;
};

class Stmt::Var : public Stmt
{
public:
    Var(const Token& name) : name(name) {}
    Var(const Token& name, std::unique_ptr<Expr> expression) : name(name), expression(std::move(expression)) {}

    Token name;
    std::unique_ptr<Expr> expression;
};
