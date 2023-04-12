cmake_minimum_required(VERSION 3.24)
project(golf)

set(CMAKE_CXX_STANDARD 20)

add_executable(golf src/golf.cpp src/lexer.cpp src/token.cpp src/logger.cpp src/file_input.cpp src/file_input.h src/parser.cpp src/parser.h src/ast.cpp src/ast.h src/input.cpp src/input.h src/repl_input.cpp src/repl_input.h src/semantic.cpp src/semantic.h src/symbol_table.cpp src/symbol_table.h src/record.cpp src/record.h src/code_gen.cpp src/code_gen.h)
