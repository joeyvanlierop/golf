#pragma once

#include <vector>
#include <map>
#include <string>

#include "input.h"

struct Record {
    std::string sig;
    std::string rt_sig;
    bool is_const;
    bool is_type;
};

class SymbolTable {
public:
    SymbolTable(Input* input);
    void define(std::string name, std::string type, int line, int column);
    Record* lookup(std::string name, int line, int column);
    void open_scope();
    void close_scope();

private:
    Input* input;
    std::vector<std::map<std::string, Record*>> scopes;
};

