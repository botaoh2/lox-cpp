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
    class Assign;
    class Logical;
    class Call;
    class Get;
    class Set;

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
    virtual void visitAssign(const Expr::Assign& expr) = 0;
    virtual void visitLogical(const Expr::Logical& expr) = 0;
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

class Expr::Assign : public Expr
{
public:
    Assign(const Token& name, std::unique_ptr<Expr> value) : name(name), value(std::move(value)) {}

    void accept(ExprVisitor& visitor) const override { visitor.visitAssign(*this); }

    Token name;
    std::unique_ptr<Expr> value;
};

class Expr::Logical : public Expr
{
public:
    Logical(std::unique_ptr<Expr> left, const Token& op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right))
    {
    }

    void accept(ExprVisitor& visitor) const override { visitor.visitLogical(*this); }

    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
};

class Expr::Call : public Expr
{
public:
    Call(std::unique_ptr<Expr> callee, const Token& paren, std::vector<std::unique_ptr<Expr>>&& arguments)
        : callee(std::move(callee)), paren(paren), arguments(std::move(arguments))
    {
    }

    void accept(ExprVisitor& visitor) const override {}

    std::unique_ptr<Expr> callee;
    Token paren;
    std::vector<std::unique_ptr<Expr>> arguments;
};

class Expr::Get : public Expr
{
public:
    Get(std::unique_ptr<Expr> object, const Token& name) : object(std::move(object)), name(name) {}

    void accept(ExprVisitor& visitor) const override {}

    std::unique_ptr<Expr> object;
    Token name;
};

class Expr::Set : public Expr
{
public:
    Set(std::unique_ptr<Expr> object, const Token& name, std::unique_ptr<Expr> value)
        : object(std::move(object)), name(name), value(std::move(value))
    {
    }

    void accept(ExprVisitor& visitor) const override {}

    std::unique_ptr<Expr> object;
    Token name;
    std::unique_ptr<Expr> value;
};
