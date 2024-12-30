#include "pch.hpp"

#include "printer.hpp"

void ExprPrinter::visitBinary(const Expr::Binary& expr)
{
    result = fmt::format("({} {} {})", expr.op.lexeme, *expr.left, *expr.right);
}

void ExprPrinter::visitGrouping(const Expr::Grouping& expr)
{
    result = fmt::format("(group {})", *expr.expression);
}

void ExprPrinter::visitLiteral(const Expr::Literal& expr)
{
    result = fmt::format("{}", expr.value.lexeme);
}

void ExprPrinter::visitUnary(const Expr::Unary& expr)
{
    result = fmt::format("({} {})", expr.op.lexeme, *expr.right);
}

std::string format_as(const Expr& expr)
{
    ExprPrinter printer;
    printer.visitExpr(expr);
    return printer.result;
}
