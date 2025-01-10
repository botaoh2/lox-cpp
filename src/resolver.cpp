#include "pch.hpp"

#include "interpreter.hpp"
#include "lox.hpp"
#include "resolver.hpp"

void Resolver::resolve(Interpreter& interpreter, const std::vector<std::unique_ptr<Stmt>>& statements)
{
    Resolver resolver(interpreter);
    resolver.resolve(statements);
}

Resolver::Resolver(Interpreter& interpreter) : interpreter(interpreter)
{
}

void Resolver::resolve(const std::vector<std::unique_ptr<Stmt>>& statements)
{
    for (const auto& stmt : statements)
        resolveStmt(*stmt);
}

void Resolver::resolveStmt(const Stmt& stmt)
{
    if (const auto* expressionStmt = dynamic_cast<const Stmt::Expression*>(&stmt))
        return resolveExpressionStmt(*expressionStmt);
    if (const auto* printStmt = dynamic_cast<const Stmt::Print*>(&stmt))
        return resolvePrintStmt(*printStmt);
    if (const auto* varStmt = dynamic_cast<const Stmt::Var*>(&stmt))
        return resolveVarStmt(*varStmt);
    if (const auto* blockStmt = dynamic_cast<const Stmt::Block*>(&stmt))
        return resolveBlockStmt(*blockStmt);
    if (const auto* ifStmt = dynamic_cast<const Stmt::If*>(&stmt))
        return resolveIfStmt(*ifStmt);
    if (const auto* whileStmt = dynamic_cast<const Stmt::While*>(&stmt))
        return resolveWhileStmt(*whileStmt);
    if (const auto* forStmt = dynamic_cast<const Stmt::For*>(&stmt))
        return resolveForStmt(*forStmt);
    if (const auto* funStmt = dynamic_cast<const Stmt::Fun*>(&stmt))
        return resolveFunStmt(*funStmt);
    if (const auto* returnStmt = dynamic_cast<const Stmt::Return*>(&stmt))
        return resolveReturnStmt(*returnStmt);
    if (const auto* classStmt = dynamic_cast<const Stmt::Class*>(&stmt))
        return resolveClassStmt(*classStmt);
}

void Resolver::resolveExpressionStmt(const Stmt::Expression& stmt)
{
    resolveExpr(*stmt.expression);
}

void Resolver::resolvePrintStmt(const Stmt::Print& stmt)
{
    resolveExpr(*stmt.expression);
}

void Resolver::resolveVarStmt(const Stmt::Var& stmt)
{
    declare(stmt.name);
    if (stmt.expression)
        resolveExpr(*stmt.expression);
    define(stmt.name);
}

void Resolver::resolveBlockStmt(const Stmt::Block& stmt)
{
    beginScope();
    resolve(stmt.statements);
    endScope();
}

void Resolver::resolveIfStmt(const Stmt::If& stmt)
{
    resolveExpr(*stmt.condition);
    resolveStmt(*stmt.ifBranch);
    if (stmt.elseBranch)
        resolveStmt(*stmt.elseBranch);
}

void Resolver::resolveWhileStmt(const Stmt::While& stmt)
{
    resolveExpr(*stmt.condition);
    resolveStmt(*stmt.body);
}

void Resolver::resolveForStmt(const Stmt::For& stmt)
{
    if (stmt.initializer)
        resolveStmt(*stmt.initializer);
    if (stmt.condition)
        resolveExpr(*stmt.condition);
    if (stmt.step)
        resolveExpr(*stmt.step);

    resolveStmt(*stmt.body);
}

void Resolver::resolveFunStmt(const Stmt::Fun& stmt)
{
    declare(stmt.name);
    define(stmt.name);
    resolveFunction(stmt, FunctionType::Function);
}

void Resolver::resolveReturnStmt(const Stmt::Return& stmt)
{
    if (m_currentType == FunctionType::None)
        error(stmt.keyword, "Can't return from top-level code.");

    if (stmt.value)
        resolveExpr(*stmt.value);
}

void Resolver::resolveClassStmt(const Stmt::Class& stmt)
{
    declare(stmt.name);
    define(stmt.name);

    beginScope();
    m_scopes.back().insert({"this", true});

    for (const auto& method : stmt.methods)
    {
        resolveFunction(*method, FunctionType::Method);
    }

    endScope();
}

void Resolver::resolveExpr(const Expr& expr)
{
    if (const auto* binaryExpr = dynamic_cast<const Expr::Binary*>(&expr))
        return resolveBinaryExpr(*binaryExpr);
    if (const auto* groupingExpr = dynamic_cast<const Expr::Grouping*>(&expr))
        return resolveGroupingExpr(*groupingExpr);
    if (const auto* literalExpr = dynamic_cast<const Expr::Literal*>(&expr))
        return resolveLiteralExpr(*literalExpr);
    if (const auto* unaryExpr = dynamic_cast<const Expr::Unary*>(&expr))
        return resolveUnaryExpr(*unaryExpr);
    if (const auto* variableExpr = dynamic_cast<const Expr::Variable*>(&expr))
        return resolveVariableExpr(*variableExpr);
    if (const auto* assignExpr = dynamic_cast<const Expr::Assign*>(&expr))
        return resolveAssignExpr(*assignExpr);
    if (const auto* logicalExpr = dynamic_cast<const Expr::Logical*>(&expr))
        return resolveLogicalExpr(*logicalExpr);
    if (const auto* callExpr = dynamic_cast<const Expr::Call*>(&expr))
        return resolveCallExpr(*callExpr);
    if (const auto* getExpr = dynamic_cast<const Expr::Get*>(&expr))
        return resolveGetExpr(*getExpr);
    if (const auto* thisExpr = dynamic_cast<const Expr::This*>(&expr))
        return resolveThisExpr(*thisExpr);
}

void Resolver::resolveBinaryExpr(const Expr::Binary& expr)
{
    resolveExpr(*expr.left);
    resolveExpr(*expr.right);
}

void Resolver::resolveGroupingExpr(const Expr::Grouping& expr)
{
    resolveExpr(*expr.expression);
}

void Resolver::resolveLiteralExpr(const Expr::Literal& expr)
{
    // do nothing
}

void Resolver::resolveUnaryExpr(const Expr::Unary& expr)
{
    resolveExpr(*expr.right);
}

void Resolver::resolveVariableExpr(const Expr::Variable& expr)
{
    if (!m_scopes.empty())
    {
        const auto& scope = m_scopes.back();
        if (auto it = scope.find(expr.name.lexeme); it != scope.end() && it->second == false)
        {
            error(expr.name, "Can't read local variable in its own initializer.");
        }
    }

    resolveLocal(expr, expr.name);
}

void Resolver::resolveAssignExpr(const Expr::Assign& expr)
{
    resolveExpr(*expr.value);
    resolveLocal(expr, expr.name);
}

void Resolver::resolveLogicalExpr(const Expr::Logical& expr)
{
    resolveExpr(*expr.left);
    resolveExpr(*expr.right);
}

void Resolver::resolveCallExpr(const Expr::Call& expr)
{
    resolveExpr(*expr.callee);

    for (const auto& arg : expr.arguments)
        resolveExpr(*arg);
}

void Resolver::resolveGetExpr(const Expr::Get& expr)
{
    resolveExpr(*expr.object);
}

void Resolver::resolveSetExpr(const Expr::Set& expr)
{
    resolveExpr(*expr.object);
    resolveExpr(*expr.value);
}

void Resolver::resolveThisExpr(const Expr::This& expr)
{
    resolveLocal(expr, expr.keyword);
}

void Resolver::beginScope()
{
    m_scopes.emplace_back();
}

void Resolver::endScope()
{
    m_scopes.pop_back();
}

void Resolver::declare(const Token& name)
{
    if (m_scopes.empty())
        return;

    auto& scope = m_scopes.back();
    if (scope.contains(name.lexeme))
        error(name, "Already a variable with this name in this scope.");
    scope[name.lexeme] = false;
}

void Resolver::define(const Token& name)
{
    if (m_scopes.empty())
        return;

    auto& scope = m_scopes.back();
    scope[name.lexeme] = true;
}

void Resolver::resolveLocal(const Expr& expr, const Token& name)
{
    for (int i = m_scopes.size() - 1; i >= 0; i--)
    {
        if (m_scopes[i].contains(name.lexeme))
        {
            interpreter.resolve(expr, m_scopes.size() - 1 - i);
            return;
        }
    }
}

void Resolver::resolveFunction(const Stmt::Fun& function, FunctionType type)
{
    FunctionType enclosingType = m_currentType;
    m_currentType = type;
    beginScope();
    for (const auto& param : function.params)
    {
        declare(param);
        define(param);
    }
    resolve(function.body);
    endScope();
    m_currentType = enclosingType;
}
