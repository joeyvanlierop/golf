#pragma once

#include <vector>
#include <map>
#include <string>

#include "input.h"

class Symbol {
};

class SymbolTable {
public:
    SymbolTable(Input* input);
    void define(std::string name, std::string type, int line, int column);
    std::string lookup(std::string name, int line, int column);
    void push_scope();
    void pop_scope();

private:
    Input* input;
    std::vector<std::map<std::string, std::string>> scopes;
};

