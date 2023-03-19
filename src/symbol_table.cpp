#include "symbol_table.h"
#include "logger.h"

SymbolTable::SymbolTable(Input *input): input(input), scopes(1) { }

void SymbolTable::define(std::string name, std::string type, int line, int column) {
    if(scopes.back().contains(name))
        Logger::error(input, line, column, name.length(), "\"" + name + "\"" + " redefined");
    scopes.back()[name] = type;
}

std::string SymbolTable::lookup(std::string name, int line, int column) {
    for (auto scope = scopes.rbegin(); scope != scopes.rend(); scope++)
        if(scope->contains(name))
            return (*scope)[name];
    Logger::error(input, line, column, name.length(), "unknown identifier \"" + name + "\"");
}

void SymbolTable::push_scope() {
    scopes.push_back({});
}

void SymbolTable::pop_scope() {
    if(scopes.size() == 1)
        Logger::error(input, 0, 0, 1, "cannot pop the global scope");
    scopes.pop_back();
}
