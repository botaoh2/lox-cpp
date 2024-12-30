#include "pch.hpp"

#include "lox.hpp"
#include "parser.hpp"
#include "token.hpp"

#include <set>

std::unique_ptr<Expr> Parser::parse(const std::vector<Token>& tokens)
{
    try
    {
        Parser parser(tokens);
        return parser.expression();
    }
    catch (const Error& e)
    {
        error(e.token, e.message);
        return std::unique_ptr<Expr>{};
    }
}

std::unique_ptr<Expr> Parser::expression()
{
    return equality();
}

std::unique_ptr<Expr> Parser::equality()
{
    std::unique_ptr<Expr> expr = comparison();

    while (match({TokenType::Bang, TokenType::BangEqual}))
    {
        const Token& op = previous();
        std::unique_ptr<Expr> right = comparison();
        expr = std::make_unique<Expr::Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::comparison()
{
    std::unique_ptr<Expr> expr = term();

    while (match({TokenType::Greater, TokenType::GreaterEqual, TokenType::Less, TokenType::LessEqual}))
    {
        const Token& op = previous();
        std::unique_ptr<Expr> right = term();
        expr = std::make_unique<Expr::Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::term()
{
    std::unique_ptr<Expr> expr = factor();

    while (match({TokenType::Minus, TokenType::Plus}))
    {
        const Token& op = previous();
        std::unique_ptr<Expr> right = factor();
        expr = std::make_unique<Expr::Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::factor()
{
    std::unique_ptr<Expr> expr = unary();

    while (match({TokenType::Slash, TokenType::Star}))
    {
        const Token& op = previous();
        std::unique_ptr<Expr> right = unary();
        expr = std::make_unique<Expr::Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::unary()
{
    if (match({TokenType::Bang, TokenType::Minus}))
    {
        const Token& op = previous();
        std::unique_ptr<Expr> right = unary();
        return std::make_unique<Expr::Unary>(op, std::move(right));
    }

    return primary();
}

std::unique_ptr<Expr> Parser::primary()
{
    if (match({TokenType::Number, TokenType::String, TokenType::True, TokenType::False, TokenType::Nil}))
        return std::make_unique<Expr::Literal>(previous());

    if (match({TokenType::LeftParen}))
    {
        auto expr = std::make_unique<Expr::Grouping>(expression());
        if (!match({TokenType::RightParen}))
        {
            throw Error(peek(), "Expecting ')'");
        }

        return expr;
    }

    throw Error(peek(), "Expecting expression");
}

bool Parser::isAtEnd() const
{
    return m_current >= m_tokens.size();
}

const Token& Parser::advance()
{
    if (isAtEnd())
        return m_tokens.back();
    return m_tokens[m_current++];
}

bool Parser::match(std::initializer_list<TokenType> expected)
{
    if (isAtEnd())
        return false;
    TokenType peekType = peek().type;
    if (std::any_of(expected.begin(), expected.end(), [&](TokenType type) { return type == peekType; }))
    {
        advance();
        return true;
    }

    return false;
}

const Token& Parser::peek() const
{
    if (isAtEnd())
        return m_tokens.back();
    return m_tokens[m_current];
}

const Token& Parser::previous() const
{
    assert(m_current >= 1);
    return m_tokens[m_current - 1];
}

void Parser::synchronize()
{
    static const std::set<TokenType> syncPoints{
        TokenType::Class,
        TokenType::Fun,
        TokenType::Var,
        TokenType::For,
        TokenType::If,
        TokenType::While,
        TokenType::Print,
        TokenType::Return,
    };

    advance();

    while (!isAtEnd())
    {
        if (previous().type == TokenType::Semicolon)
            return;

        if (syncPoints.contains(peek().type))
            return;

        advance();
    }
}
