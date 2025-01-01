#pragma once

#include "token.hpp"

class ExprVisitor;

class Expr
{
public:
    class Binary;
    class Grouping;
    class Literal;
    class Unary;
    class Variable;

    virtual ~Expr() = default;
    virtual void accept(ExprVisitor& visitor) const = 0;
};

class ExprVisitor
{
public:
    friend class Expr;
    virtual ~ExprVisitor() = default;

    void visitExpr(const Expr& expr) { expr.accept(*this); }

protected:
    virtual void visitBinary(const Expr::Binary& expr) = 0;
    virtual void visitGrouping(const Expr::Grouping& expr) = 0;
    virtual void visitLiteral(const Expr::Literal& expr) = 0;
    virtual void visitUnary(const Expr::Unary& expr) = 0;
    virtual void visitVariable(const Expr::Variable& expr) = 0;
};

class Expr::Binary : public Expr
{
public:
    Binary(std::unique_ptr<Expr> left, const Token& op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right))
    {
    }

    void accept(ExprVisitor& visitor) const override { visitor.visitBinary(*this); }

    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
};

class Expr::Grouping : public Expr
{
public:
    Grouping(std::unique_ptr<Expr> expression) : expression(std::move(expression)) {}

    void accept(ExprVisitor& visitor) const override { visitor.visitGrouping(*this); }

    std::unique_ptr<Expr> expression;
};

class Expr::Literal : public Expr
{
public:
    Literal(const Token& value) : value(value) {}

    void accept(ExprVisitor& visitor) const override { visitor.visitLiteral(*this); }

    Token value;
};

class Expr::Unary : public Expr
{
public:
    Unary(const Token& op, std::unique_ptr<Expr> right) : op(op), right(std::move(right)) {}

    void accept(ExprVisitor& visitor) const override { visitor.visitUnary(*this); }

    Token op;
    std::unique_ptr<Expr> right;
};

class Expr::Variable : public Expr
{
public:
    Variable(const Token& name) : name(name) {}

    void accept(ExprVisitor& visitor) const override { visitor.visitVariable(*this); }

    Token name;
};
