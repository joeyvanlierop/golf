cmake_minimum_required(VERSION 3.24)
project(golf)

set(CMAKE_CXX_STANDARD 20)

add_executable(golf src/golf.cpp src/lexer.cpp src/token.cpp src/logger.cpp src/filereader.cpp src/parser.cpp src/parser.h src/ast.cpp src/ast.h)
