#include "pch.hpp"

#include "lox.hpp"

#include "scanner.hpp"

#include <fstream>
#include <sstream>

bool hadError = false;

static void run(std::string_view code)
{
    auto tokens = Scanner::scanTokens(code);
    for (const auto& token : tokens)
    {
        std::cout << token << '\n';
    }
}

void runFile(const char* filename)
{
    std::ifstream file(filename);
    if (!file)
    {
        std::cerr << "Error opening file: " << filename << '\n';
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

void error(int line, std::string_view message)
{
    std::cerr << "[line " << line << "] Error: " << message << std::endl;
}
