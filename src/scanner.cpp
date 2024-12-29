#include "pch.hpp"

#include "scanner.hpp"

#include "lox.hpp"
#include "token.hpp"

#include <cctype>
#include <unordered_map>

static bool isDigit(char c)
{
    return c >= '0' && c <= '9';
}

static bool isAlpha(char c)
{
    return std::isalpha(c) || c == '_';
}

static bool isAlphaNum(char c)
{
    return isAlpha(c) || isDigit(c);
}

static const std::unordered_map<std::string_view, TokenType> keywords{
    {"and", TokenType::And},
    {"class", TokenType::Class},
    {"else", TokenType::Else},
    {"false", TokenType::False},
    {"fun", TokenType::Fun},
    {"for", TokenType::For},
    {"if", TokenType::If},
    {"nil", TokenType::Nil},
    {"or", TokenType::Or},
    {"print", TokenType::Print},
    {"return", TokenType::Return},
    {"super", TokenType::Super},
    {"this", TokenType::This},
    {"true", TokenType::True},
    {"var", TokenType::Var},
    {"while", TokenType::While},
};

std::vector<Token> Scanner::scanTokens(std::string_view source)
{
    Scanner scanner(source);
    return scanner.scanTokens();
}

std::vector<Token> Scanner::scanTokens()
{
    while (!isAtEnd())
    {
        m_start = m_current;
        scanToken();
    }

    m_tokens.push_back(Token(TokenType::Eof, "", m_line));
    return std::move(m_tokens);
}

void Scanner::scanToken()
{
    char c = advance();
    switch (c)
    {
    case '(':
        addToken(TokenType::LeftParen);
        break;
    case ')':
        addToken(TokenType::RightParen);
        break;
    case '{':
        addToken(TokenType::LeftBrace);
        break;
    case '}':
        addToken(TokenType::RightBrace);
        break;
    case ',':
        addToken(TokenType::Comma);
        break;
    case '.':
        addToken(TokenType::Dot);
        break;
    case '-':
        addToken(TokenType::Minus);
        break;
    case '+':
        addToken(TokenType::Plus);
        break;
    case ';':
        addToken(TokenType::Semicolon);
        break;
    case '*':
        addToken(TokenType::Star);
        break;
    case '!':
        addToken(match('=') ? TokenType::BangEqual : TokenType::Bang);
        break;
    case '=':
        addToken(match('=') ? TokenType::EqualEqual : TokenType::Equal);
        break;
    case '<':
        addToken(match('=') ? TokenType::LessEqual : TokenType::Less);
        break;
    case '>':
        addToken(match('=') ? TokenType::GreaterEqual : TokenType::Greater);
        break;
    case '/':
        if (match('/'))
        {
            while (peek() != '\n' && !isAtEnd())
                advance();
        }
        else
        {
            addToken(TokenType::Slash);
        }
    case ' ':
    case '\r':
    case '\t':
        break;
    case '\n':
        m_line++;
        break;

    case '"':
        scanStringToken();
        break;

    default:
        if (isDigit(c))
            scanNumberToken();
        else if (isAlpha(c))
            scanIdentifierToken();
        else
            error(m_line, "Unexpected character.");
    }
    // TODO
}

void Scanner::addToken(TokenType type)
{
    std::string lexeme(m_source.substr(m_start, m_current - m_start));
    m_tokens.emplace_back(type, lexeme, m_line);
}

void Scanner::scanStringToken()
{
    while (peek() != '"' && !isAtEnd())
    {
        if (peek() == '\n')
            m_line++;
        advance();
    }

    if (isAtEnd())
    {
        error(m_line, "Unterminated string.");
        return;
    }

    advance();

    addToken(TokenType::String);
}

void Scanner::scanNumberToken()
{
    while (isDigit(peek()))
        advance();

    if (peek() == '.' && isDigit(peekNext()))
    {
        advance();

        while (isDigit(peek()))
            advance();
    }

    addToken(TokenType::Number);
}

void Scanner::scanIdentifierToken()
{
    while (isAlphaNum(peek()))
        advance();

    const auto text = m_source.substr(m_start, m_current - m_start);
    if (auto it = keywords.find(text); it != keywords.end())
        addToken(it->second);
    else
        addToken(TokenType::Identifier);
}

bool Scanner::isAtEnd() const
{
    return m_current >= m_source.size();
}

char Scanner::advance()
{
    assert(m_current < m_source.size());
    return m_source[m_current++];
}

bool Scanner::match(char expected)
{
    if (isAtEnd())
        return false;
    if (m_source[m_current] != expected)
        return false;

    m_current++;
    return true;
}

char Scanner::peek() const
{
    if (isAtEnd())
        return '\0';
    return m_source[m_current];
}

char Scanner::peekNext() const
{
    if (m_current + 1 >= m_source.size())
        return '\0';
    return m_source[m_current + 1];
}
