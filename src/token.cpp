#include "pch.hpp"

#include "token.hpp"

static auto tokenTypeStrings = std::to_array({
    // Single-character tokens
    "LeftParen",
    "RightParen",
    "LeftBrace",
    "RightBrace",
    "Comma",
    "Dot",
    "Minus",
    "Plus",
    "Semicolon",
    "Slash",
    "Star",

    // One or two character tokens
    "Bang",
    "BangEqual",
    "Equal",
    "EqualEqual",
    "Greater",
    "GreaterEqual",
    "Less",
    "LessEqual",

    // Literals
    "Identifier",
    "String",
    "Number",

    // Keywords
    "And",
    "Class",
    "Else",
    "False",
    "Fun",
    "For",
    "If",
    "Nil",
    "Or",
    "Print",
    "Return",
    "Super",
    "This",
    "True",
    "Var",
    "While",

    // EOF
    "Eof",
});

std::ostream& operator<<(std::ostream& os, TokenType type)
{
    return os << tokenTypeStrings[static_cast<int>(type)];
}

std::ostream& operator<<(std::ostream& os, const Token& token)
{
    return os << token.type << " " << token.lexeme << " " << token.line;
}
