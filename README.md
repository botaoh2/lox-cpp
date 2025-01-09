# Lox-CPP

A C++ implementation of the Lox programming language from Robert Nystrom's book, [Crafting Interpreters](https://craftinginterpreters.com/). This project implements the Lox language using C++, showcasing an interpreter for the dynamically typed, object-oriented language.

## Features

* Fully functional interpreter for the Lox programming language.
* Supports:
    * Variables, control flow (if, while, for).
    * Functions, closures, and classes.
    * Dynamic typing and runtime error checking.
    * Lexical scoping with proper variable resolution.
* Error reporting for syntax and runtime errors.

## Building the project

### Prerequisites

* C++20
* cmake 3.28 or later

### Building the project

```
mkdir build
cd build
cmake ..
cmake --build .
```
