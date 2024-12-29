#pragma once

#include "token.hpp"

class Scanner
{
public:
    static std::vector<Token> scanTokens(std::string_view source);

private:
    Scanner(std::string_view source) : m_source(source) {}

    std::vector<Token> scanTokens();
    void scanToken();
    void addToken(TokenType type);
    void scanStringToken();
    void scanNumberToken();
    void scanIdentifierToken();

    bool isAtEnd() const;
    char advance();
    bool match(char expected);
    char peek() const;
    char peekNext() const;

    std::string_view m_source;
    std::vector<Token> m_tokens;

    int m_start = 0;
    int m_current = 0;
    int m_line = 1;
};
