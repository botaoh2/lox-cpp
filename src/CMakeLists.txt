
set(SRC_FILES
main.cpp
)

add_executable(lox ${SRC_FILES})
set_target_properties(lox PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
