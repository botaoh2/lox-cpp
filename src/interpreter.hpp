#pragma once

#include "expr.hpp"
#include "value.hpp"

class Interpreter
{
public:
    void interpret(const Expr& expr);

private:
    class Error
    {
    public:
        Error(const Token& token, const std::string& message) : token(token), message(message) {}

        Token token;
        std::string message;
    };

    Value eval(const Expr& expr);
    Value eval(const Expr::Binary& expr);
    Value eval(const Expr::Grouping& expr);
    Value eval(const Expr::Literal& expr);
    Value eval(const Expr::Unary& expr);

    bool isTruthy(const Value& value);
    void checkNumber(const Token& token, const Value& value);
    void checkNumber(const Token& token, const Value& left, const Value& right);
    void checkString(const Token& token, const Value& value);
};
