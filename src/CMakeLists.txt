
set(SRC_FILES
environment.cpp
expr.cpp
interpreter.cpp
lox.cpp
main.cpp
parser.cpp
printer.cpp
resolver.cpp
scanner.cpp
token.cpp
value.cpp
)

add_executable(lox ${SRC_FILES})
set_target_properties(lox PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

target_precompile_headers(lox PRIVATE pch.hpp)
target_link_libraries(lox PRIVATE fmt::fmt)
