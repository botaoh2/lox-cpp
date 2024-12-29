#pragma once

enum class TokenType
{
    // Single-character tokens
    LeftParen = 0,
    RightParen,
    LeftBrace,
    RightBrace,
    Comma,
    Dot,
    Minus,
    Plus,
    Semicolon,
    Slash,
    Star,

    // One or two character tokens
    Bang,
    BangEqual,
    Equal,
    EqualEqual,
    Greater,
    GreaterEqual,
    Less,
    LessEqual,

    // Literals
    Identifier,
    String,
    Number,

    // Keywords
    And,
    Class,
    Else,
    False,
    Fun,
    For,
    If,
    Nil,
    Or,
    Print,
    Return,
    Super,
    This,
    True,
    Var,
    While,

    // EOF
    Eof
};

struct Token
{
    TokenType type;
    std::string lexeme;
    int line;

    Token(TokenType type, const std::string& lexeme, int line) : type(type), lexeme(lexeme), line(line) {}
};

std::ostream& operator<<(std::ostream& os, TokenType type);
std::ostream& operator<<(std::ostream& os, const Token& token);

std::string format_as(TokenType type);
std::string format_as(const Token& token);
