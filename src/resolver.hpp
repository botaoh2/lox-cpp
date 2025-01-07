#pragma once

#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"

#include <unordered_map>

class Interpreter;

class Resolver
{
public:
    Resolver(Interpreter& interpreter);

    void resolve(const std::vector<std::unique_ptr<Stmt>>& statements);

private:
    enum class FunctionType
    {
        None,
        Function,
    };

    void resolveStmt(const Stmt& stmt);
    void resolveExpressionStmt(const Stmt::Expression& stmt);
    void resolvePrintStmt(const Stmt::Print& stmt);
    void resolveVarStmt(const Stmt::Var& stmt);
    void resolveBlockStmt(const Stmt::Block& stmt);
    void resolveIfStmt(const Stmt::If& stmt);
    void resolveWhileStmt(const Stmt::While& stmt);
    void resolveForStmt(const Stmt::For& stmt);
    void resolveFunStmt(const Stmt::Fun& stmt);
    void resolveReturnStmt(const Stmt::Return& stmt);

    void resolveExpr(const Expr& expr);
    void resolveBinaryExpr(const Expr::Binary& expr);
    void resolveGroupingExpr(const Expr::Grouping& expr);
    void resolveLiteralExpr(const Expr::Literal& expr);
    void resolveUnaryExpr(const Expr::Unary& expr);
    void resolveVariableExpr(const Expr::Variable& expr);
    void resolveAssignExpr(const Expr::Assign& expr);
    void resolveLogicalExpr(const Expr::Logical& expr);
    void resolveCallExpr(const Expr::Call& expr);

    void beginScope();
    void endScope();

    void declare(const Token& name);
    void define(const Token& name);

    void resolveFunction(const Stmt::Fun& function, FunctionType type);
    void resolveLocal(const Expr& expr, const Token& name);

    Interpreter& interpreter;
    std::vector<std::unordered_map<std::string, bool>> m_scopes;
    FunctionType m_currentType;
};
