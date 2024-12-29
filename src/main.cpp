#include "pch.hpp"

#include "lox.hpp"

int main(int argc, char** argv)
{
    if (argc > 2)
    {
        fmt::println(stderr, "Usage: lox [script]");
        std::exit(1);
    }
    else if (argc == 2)
    {
        runFile(argv[1]);
    }
    else
    {
        runPrompt();
    }
}
