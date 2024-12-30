#pragma once

#include "token.hpp"

void runFile(const char* filename);
void runPrompt();

void error(int line, std::string_view message);
void error(const Token& token, std::string_view message);
