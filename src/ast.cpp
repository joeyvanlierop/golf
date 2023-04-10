#include <iostream>
#include "ast.h"

AST::AST(int line, int column) : line(line), column(column), sym(nullptr) {}

void AST::print() {
    std::cout << "TODO" << std::endl;
}