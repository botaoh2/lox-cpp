#include "pch.hpp"

#include "lox.hpp"

#include "parser.hpp"
#include "printer.hpp"
#include "scanner.hpp"

#include <fstream>
#include <sstream>

bool hadError = false;

static void run(std::string_view code)
{
    auto tokens = Scanner::scanTokens(code);
    auto expr = Parser::parse(tokens);
    if (expr)
        fmt::println("{}", *expr);
}

void runFile(const char* filename)
{
    std::ifstream file(filename);
    if (!file)
    {
        fmt::println(stderr, "Error opening file: {}", filename);
        std::exit(1);
    }

    std::stringstream ss;
    ss << file.rdbuf();

    run(ss.str());

    if (hadError)
        std::exit(1);
}

void runPrompt()
{
    std::string line;
    while (true)
    {
        std::cerr << "> " << std::flush;
        if (!std::getline(std::cin, line))
            break;
        run(line);
        hadError = false;
    }
}

static void reportError(int line, std::string_view where, std::string_view message)
{
    fmt::println(stderr, "[line {}] Error{}: {}", line, where, message);
    hadError = true;
}

void error(int line, std::string_view message)
{
    fmt::println(stderr, "[line {}] Error: {}", line, message);
}

void error(const Token& token, std::string_view message)
{
    if (token.type == TokenType::Eof)
    {
        reportError(token.line, " at end", message);
    }
    else
    {
        reportError(token.line, fmt::format(" at '{}'", token.lexeme), message);
    }
}
