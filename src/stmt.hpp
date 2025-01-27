#pragma once

#include "expr.hpp"
#include "token.hpp"

class Stmt
{
public:
    class Expression;
    class Print;
    class Var;
    class Block;
    class If;
    class While;
    class For;
    class Fun;
    class Return;
    class Class;

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

class Stmt::Block : public Stmt
{
public:
    Block() = default;
    Block(std::vector<std::unique_ptr<Stmt>>&& statements) : statements(std::move(statements)) {}

    std::vector<std::unique_ptr<Stmt>> statements;
};

class Stmt::If : public Stmt
{
public:
    If(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> ifBranch, std::unique_ptr<Stmt> elseBranch = nullptr)
        : condition(std::move(condition)), ifBranch(std::move(ifBranch)), elseBranch(std::move(elseBranch))
    {
        assert(this->condition);
        assert(this->ifBranch);
    }

    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> ifBranch;
    std::unique_ptr<Stmt> elseBranch;
};

class Stmt::While : public Stmt
{
public:
    While(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body))
    {
        assert(this->condition);
        assert(this->body);
    }

    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;
};

class Stmt::For : public Stmt
{
public:
    For(std::unique_ptr<Stmt> initializer,
        std::unique_ptr<Expr> condition,
        std::unique_ptr<Expr> step,
        std::unique_ptr<Stmt> body)
        : initializer(std::move(initializer)),
          condition(std::move(condition)),
          step(std::move(step)),
          body(std::move(body))
    {
        assert(this->body);
    }

    std::unique_ptr<Stmt> initializer;
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> step;

    std::unique_ptr<Stmt> body;
};

class Stmt::Fun : public Stmt
{
public:
    Fun(const Token& name, std::vector<Token> params, std::vector<std::unique_ptr<Stmt>> body)
        : name(name), params(std::move(params)), body(std::move(body))
    {
    }

    Token name;
    std::vector<Token> params;
    std::vector<std::unique_ptr<Stmt>> body;
};

class Stmt::Return : public Stmt
{
public:
    Return(const Token& keyword, std::unique_ptr<Expr> value = nullptr) : keyword(keyword), value(std::move(value))
    {
        assert(keyword.type == TokenType::Return);
    }

    Token keyword;
    std::unique_ptr<Expr> value;
};

class Stmt::Class : public Stmt
{
public:
    Class(const Token& name, std::vector<std::unique_ptr<Stmt::Fun>> methods) : name(name), methods(std::move(methods))
    {
    }

    Token name;
    std::vector<std::unique_ptr<Stmt::Fun>> methods;
};
