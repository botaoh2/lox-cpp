#pragma once

#include "environment.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "value.hpp"

#include <unordered_map>

struct Return
{
    Return(const Value& value) : value(value) {}

    Value value;
};

class Interpreter
{
public:
    friend class LoxFunction;
    Interpreter();

    void interpret(const Stmt& stmt);
    Value interpret(const Expr& expr);
    void resolve(const Expr& expr, int scope);

private:
    void exec(const Stmt& stmt);
    void exec(const Stmt::Print& stmt);
    void exec(const Stmt::Expression& stmt);
    void exec(const Stmt::Var& stmt);
    void exec(const Stmt::Block& stmt);
    void exec(const Stmt::If& stmt);
    void exec(const Stmt::While& stmt);
    void exec(const Stmt::For& stmt);
    void exec(const Stmt::Fun& stmt);
    void exec(const Stmt::Return& stmt);
    void exec(const Stmt::Class& stmt);

    Value eval(const Expr& expr);
    Value eval(const Expr::Binary& expr);
    Value eval(const Expr::Grouping& expr);
    Value eval(const Expr::Literal& expr);
    Value eval(const Expr::Unary& expr);
    Value eval(const Expr::Variable& expr);
    Value eval(const Expr::Assign& expr);
    Value eval(const Expr::Logical& expr);
    Value eval(const Expr::Call& expr);
    Value eval(const Expr::Get& expr);
    Value eval(const Expr::Set& expr);
    Value eval(const Expr::This& expr);

    void executeBlock(const std::vector<std::unique_ptr<Stmt>>& statements, std::shared_ptr<Environment> env);

    bool isTruthy(const Value& value);
    bool isEqual(const Value& left, const Value& right);
    void checkNumber(const Token& token, const Value& value);
    void checkNumber(const Token& token, const Value& left, const Value& right);
    void checkString(const Token& token, const Value& value);

    Value lookupVariable(const Token& name, const Expr& expr);

    std::shared_ptr<Environment> m_global = std::make_shared<Environment>();
    std::shared_ptr<Environment> m_environment = m_global;
    std::unordered_map<const Expr*, int> m_locals;
};
