
set(SRC_FILES
expr.cpp
lox.cpp
main.cpp
parser.cpp
printer.cpp
scanner.cpp
token.cpp
)

add_executable(lox ${SRC_FILES})
set_target_properties(lox PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

target_precompile_headers(lox PRIVATE pch.hpp)
target_link_libraries(lox PRIVATE fmt::fmt)
