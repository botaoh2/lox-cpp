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
    if (match({TokenType::Class}))
        return classDeclaration();
    if (match({TokenType::Fun}))
        return funDeclaration();
    if (match({TokenType::Var}))
        return varDeclaration();
    return statement();
}

std::unique_ptr<Stmt> Parser::classDeclaration()
{
    const auto& name = consume(TokenType::Identifier, "Expecting class name");
    std::vector<std::unique_ptr<Stmt::Fun>> methods;
    consume(TokenType::LeftBrace, "Expecting '{' before class body");
    while (match({TokenType::Identifier}))
    {
        const Token& methodName = previous();
        consume(TokenType::LeftParen, "Expecting '(' after method name.");
        std::vector<Token> params;
        if (peek().type != TokenType::RightParen)
            params = parameters();
        consume(TokenType::RightParen, "Expecting ')' after method parameters.");

        consume(TokenType::LeftBrace, "Expecting '{' after function declaration.");
        auto body = block();

        methods.emplace_back(std::make_unique<Stmt::Fun>(methodName, std::move(params), std::move(body)));
    }
    consume(TokenType::RightBrace, "Expecting '}' after class body");

    return std::make_unique<Stmt::Class>(name, std::move(methods));
}

std::unique_ptr<Stmt> Parser::funDeclaration()
{
    const Token& name = consume(TokenType::Identifier, "Expecting identifier after 'fun'.");
    consume(TokenType::LeftParen, "Expecting '(' after function name.");
    std::vector<Token> params;
    if (peek().type != TokenType::RightParen)
        params = parameters();
    consume(TokenType::RightParen, "Expecting ')' after function parameters.");
    consume(TokenType::LeftBrace, "Expecting '{' after function declaration.");

    auto body = block();

    return std::make_unique<Stmt::Fun>(name, std::move(params), std::move(body));
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
    if (match({TokenType::Return}))
        return returnStatement();
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

std::unique_ptr<Stmt> Parser::returnStatement()
{
    const Token& keyword = previous();

    std::unique_ptr<Expr> value;
    if (peek().type != TokenType::Semicolon)
    {
        value = expression();
    }

    consume(TokenType::Semicolon, "Expect ';' after return value.");
    return std::make_unique<Stmt::Return>(keyword, std::move(value));
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
        else if (auto getExpr = dynamic_cast<Expr::Get*>(left.get()); getExpr != nullptr)
        {
            return std::make_unique<Expr::Set>(std::move(getExpr->object), getExpr->name, std::move(value));
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

    return call();
}

std::unique_ptr<Expr> Parser::call()
{
    auto expr = primary();

    while (true)
    {
        if (match({TokenType::LeftParen}))
        {
            std::vector<std::unique_ptr<Expr>> args;
            if (peek().type != TokenType::RightParen)
            {
                args = arguments();
            }

            const auto& paren = consume(TokenType::RightParen, "Expecting ')' after arguments");
            expr = std::make_unique<Expr::Call>(std::move(expr), paren, std::move(args));
        }
        else if (match({TokenType::Dot}))
        {
            const Token& name = consume(TokenType::Identifier, "Expecting property name after '.'");
            expr = std::make_unique<Expr::Get>(std::move(expr), name);
        }
        else
        {
            break;
        }
    }

    return expr;
}

std::unique_ptr<Expr> Parser::primary()
{
    if (match({TokenType::Number, TokenType::String, TokenType::True, TokenType::False, TokenType::Nil}))
        return std::make_unique<Expr::Literal>(previous());

    if (match({TokenType::This}))
        return std::make_unique<Expr::This>(previous());

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

std::vector<Token> Parser::parameters()
{
    std::vector<Token> tokens;
    tokens.emplace_back(consume(TokenType::Identifier, "Expecting identifier."));

    while (match({TokenType::Comma}))
    {
        tokens.emplace_back(consume(TokenType::Identifier, "Expecting identifier."));
        if (tokens.size() >= 255)
            error(peek(), "Can't have more than 255 parameters.");
    }

    return tokens;
}

std::vector<std::unique_ptr<Expr>> Parser::arguments()
{
    std::vector<std::unique_ptr<Expr>> args;
    args.emplace_back(expression());

    while (match({TokenType::Comma}))
    {
        args.emplace_back(expression());
        if (args.size() >= 255)
            error(peek(), "Can't have more than 255 arguments");
    }

    return args;
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
