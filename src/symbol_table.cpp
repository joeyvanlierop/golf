#include "symbol_table.h"
#include "logger.h"

SymbolTable::SymbolTable(Input *input): input(input), scopes(1) { }

void SymbolTable::define(std::string name, std::string type, int line, int column) {
    // Check if a record with the given name already exists
    if(scopes.back().contains(name))
        Logger::error(input, line, column, name.length(), "\"" + name + "\"" + " redefined");

    // Create and insert new record
    // TODO:
    //  This is definitely going to leak memory
    //  Not exactly sure how to fix it yet, so its going a problem for later (or never)
    auto new_record = new Record();
    new_record->sig = type;
    scopes.back()[name] = new_record;
}

Record* SymbolTable::lookup(std::string name, int line, int column) {
    // Find record, starting at the top-most scope
    for (int i = scopes.size() - 1; i >= 0; i--)
        if(scopes[i].contains(name))
            return scopes[i][name];

    // Error if no record exists
    Logger::error(input, line, column, name.length(), "unknown identifier \"" + name + "\"");
}

void SymbolTable::open_scope() {
    scopes.push_back({});
}

void SymbolTable::close_scope() {
    if(scopes.size() == 1)
        Logger::error(input, 0, 0, 1, "cannot pop the universe scope");
    scopes.pop_back();
}
