#include "pch.hpp"

#include "lox.hpp"
#include "parser.hpp"
#include "token.hpp"

#include <set>

std::vector<std::unique_ptr<Stmt>> Parser::parse(const std::vector<Token>& tokens)
{
    std::vector<std::unique_ptr<Stmt>> statements;

    try
    {
        Parser parser(tokens);
        statements = parser.program();
    }
    catch (const Error& e)
    {
        error(e.token, e.message);
    }

    return statements;
}

std::vector<std::unique_ptr<Stmt>> Parser::program()
{
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd())
        statements.emplace_back(declaration());
    return statements;
}

std::vector<std::unique_ptr<Stmt>> Parser::block()
{
    std::vector<std::unique_ptr<Stmt>> statements;

    while (!isAtEnd() && peek().type != TokenType::RightBrace)
    {
        statements.emplace_back(declaration());
    }

    consume(TokenType::RightBrace, "Expect '}' after block.");
    return statements;
}

std::unique_ptr<Stmt> Parser::declaration()
{
    if (match({TokenType::Var}))
        return varDeclaration();
    return statement();
}

std::unique_ptr<Stmt> Parser::varDeclaration()
{
    const Token& name = consume(TokenType::Identifier, "Expecting variable name");
    std::unique_ptr<Expr> expr;

    if (match({TokenType::Equal}))
    {
        expr = expression();
    }

    auto stmt = std::make_unique<Stmt::Var>(name, std::move(expr));
    consume(TokenType::Semicolon, "Expecting ';' after expression");
    return stmt;
}

std::unique_ptr<Stmt> Parser::statement()
{
    if (match({TokenType::For}))
        return forStatement();
    if (match({TokenType::If}))
        return ifStatement();
    if (match({TokenType::Print}))
        return printStatement();
    if (match({TokenType::While}))
        return whileStatement();
    if (match({TokenType::LeftBrace}))
        return std::make_unique<Stmt::Block>(block());
    return expressionStatement();
}

std::unique_ptr<Stmt> Parser::printStatement()
{
    auto expr = expression();
    auto stmt = std::make_unique<Stmt::Print>(std::move(expr));
    consume(TokenType::Semicolon, "Expecting ';' after value");
    return stmt;
}

std::unique_ptr<Stmt> Parser::expressionStatement()
{
    auto expr = expression();
    auto stmt = std::make_unique<Stmt::Expression>(std::move(expr));
    consume(TokenType::Semicolon, "Expecting ';' after expression");
    return stmt;
}

std::unique_ptr<Stmt> Parser::ifStatement()
{
    consume(TokenType::LeftParen, "Expecting '(' after 'if'.");
    auto expr = expression();
    consume(TokenType::RightParen, "Expecting ')' after if condition.");
    auto ifBranch = statement();

    std::unique_ptr<Stmt> elseBranch;
    if (match({TokenType::Else}))
        elseBranch = statement();

    return std::make_unique<Stmt::If>(std::move(expr), std::move(ifBranch), std::move(elseBranch));
}

std::unique_ptr<Stmt> Parser::whileStatement()
{
    consume(TokenType::LeftParen, "Expecting '(' after 'if'.");
    auto expr = expression();
    consume(TokenType::RightParen, "Expecting ')' after if condition.");
    auto body = statement();

    return std::make_unique<Stmt::While>(std::move(expr), std::move(body));
}

std::unique_ptr<Stmt> Parser::forStatement()
{
    consume(TokenType::LeftParen, "Expecting '(' after 'for'.");

    std::unique_ptr<Stmt> initializer;
    if (match({TokenType::Var}))
        initializer = varDeclaration();
    else if (!match({TokenType::Semicolon}))
        initializer = expressionStatement();

    std::unique_ptr<Expr> condition;
    if (peek().type != TokenType::Semicolon)
        condition = expression();
    consume(TokenType::Semicolon, "Expecting ';' after for condition.");

    std::unique_ptr<Expr> step;
    if (peek().type != TokenType::RightParen)
        step = expression();
    consume(TokenType::RightParen, "Expecting ')' after for step.");

    auto body = statement();

    return std::make_unique<Stmt::For>(std::move(initializer), std::move(condition), std::move(step), std::move(body));
}

std::unique_ptr<Expr> Parser::expression()
{
    return assignment();
}

std::unique_ptr<Expr> Parser::assignment()
{
    auto left = logicOr();
    if (match({TokenType::Equal}))
    {
        const Token& equalToken = previous();
        auto value = assignment();

        if (auto variableExpr = dynamic_cast<Expr::Variable*>(left.get()); variableExpr != nullptr)
        {
            return std::make_unique<Expr::Assign>(variableExpr->name, std::move(value));
        }

        error(equalToken, "Invalid assignment target");
    }

    return left;
}

std::unique_ptr<Expr> Parser::logicOr()
{
    std::unique_ptr<Expr> expr = logicAnd();

    while (match({TokenType::Or}))
    {
        const Token& op = previous();
        std::unique_ptr<Expr> right = logicAnd();
        expr = std::make_unique<Expr::Logical>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::logicAnd()
{
    std::unique_ptr<Expr> expr = equality();

    while (match({TokenType::And}))
    {
        const Token& op = previous();
        std::unique_ptr<Expr> right = equality();
        expr = std::make_unique<Expr::Logical>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::equality()
{
    std::unique_ptr<Expr> expr = comparison();

    while (match({TokenType::EqualEqual, TokenType::BangEqual}))
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

    if (match({TokenType::Identifier}))
        return std::make_unique<Expr::Variable>(previous());

    if (match({TokenType::LeftParen}))
    {
        auto expr = std::make_unique<Expr::Grouping>(expression());
        consume(TokenType::RightParen, "Expecting ')'");
        return expr;
    }

    throw Error(peek(), "Expecting expression");
}

bool Parser::isAtEnd() const
{
    return m_current >= m_tokens.size() - 1;
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

const Token& Parser::consume(TokenType expected, const char* message)
{
    const auto& token = peek();
    if (token.type != expected)
        throw Error(token, message);

    return advance();
}
