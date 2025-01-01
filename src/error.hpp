#pragma once

#include "token.hpp"

class RuntimeError
{
public:
    RuntimeError(const Token& token, const std::string& message) : token(token), message(message) {}

    Token token;
    std::string message;
};
