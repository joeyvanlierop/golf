#pragma once

#include <vector>
#include <map>
#include <string>
#include <iostream>

#include "input.h"
#include "ast.h"
#include "record.h"

class SymbolTable {
public:
    SymbolTable(Input* input);
	Record* define(AST* ast, Record record);
	Record* define(std::string name, Record record);
    Record* lookup(AST* ast);
    Record* lookup(std::string name);
    void open_scope();
    void close_scope();
    void print();
    void print_scope(int i);

private:
    Input* input;
    std::vector<std::map<std::string, Record>> scopes;
};

