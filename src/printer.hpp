#pragma once

#include "expr.hpp"

class ExprPrinter : public ExprVisitor
{
public:
    std::string result;

protected:
    void visitBinary(const Expr::Binary& expr) override;
    void visitGrouping(const Expr::Grouping& expr) override;
    void visitLiteral(const Expr::Literal& expr) override;
    void visitUnary(const Expr::Unary& expr) override;
};

std::string format_as(const Expr& expr);
