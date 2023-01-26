cmake_minimum_required(VERSION 3.24)
project(cpsc_411)

set(CMAKE_CXX_STANDARD 20)

add_executable(cpsc_411 src/golf.cpp src/lexer.cpp src/token.cpp)
