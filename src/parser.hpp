#pragma once

#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"

class Parser
{
public:
    static std::vector<std::unique_ptr<Stmt>> parse(const std::vector<Token>& tokens);

private:
    class Error
    {
    public:
        Error(const Token& token, const std::string& message) : token(token), message(message) {}

        Token token;
        std::string message;
    };

    Parser(const std::vector<Token>& tokens) : m_tokens(tokens)
    {
        assert(tokens.size() > 0);
        assert(tokens.back().type == TokenType::Eof);
    }

    std::vector<std::unique_ptr<Stmt>> program();
    std::vector<std::unique_ptr<Stmt>> block();

    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> varDeclaration();
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> printStatement();
    std::unique_ptr<Stmt> expressionStatement();
    std::unique_ptr<Stmt> ifStatement();

    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> assignment();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();

    bool isAtEnd() const;
    const Token& advance();
    bool match(std::initializer_list<TokenType> expected);
    const Token& peek() const;
    const Token& previous() const;
    void synchronize();
    const Token& consume(TokenType expected, const char* message);

    const std::vector<Token>& m_tokens;
    int m_current = 0;
};
