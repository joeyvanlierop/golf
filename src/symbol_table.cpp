#include <iostream>
#include <iomanip>

#include "symbol_table.h"
#include "logger.h"

SymbolTable::SymbolTable(Input *input): input(input), scopes(0) { }

Record* SymbolTable::define(AST* ast, Record record) {
    // What info do we need from the ast
    auto name = ast->attr;
    auto line = ast->line;
    auto column = ast->column;

    // Check if a record with the given name already exists
    if (scopes.back().count(name))
        Logger::error(input, line, column, name.length(), "\"" + name + "\"" + " redefined");

    // Create and insert new record
    return define(name, record);
}

Record* SymbolTable::define(std::string name, Record record) {
    // Create and insert new record
    return &(scopes.back()[name] = record);
}


Record* SymbolTable::lookup(AST* ast) {
    // What info do we need from the ast
    auto name = ast->attr;
    auto line = ast->line;
    auto column = ast->column;

    // Find record, starting at the top-most scope
    for (int i = scopes.size() - 1; i >= 0; i--)
        if(scopes[i].count(name))
            return &scopes[i][name];

    // Error if no record exists
    Logger::error(input, line, column, name.length(), "unknown identifier \"" + name + "\"");
	throw 0;
}

Record* SymbolTable::lookup(std::string name) {
    // Find record, starting at the top-most scope
    for (int i = scopes.size() - 1; i >= 0; i--)
        if(scopes[i].count(name))
            return &scopes[i][name];

    // Error if no record exists
    Logger::error(input, 0, 0, name.length(), "unknown identifier \"" + name + "\"");
	throw 0;
}

void SymbolTable::open_scope() {
    scopes.push_back({});
}

void SymbolTable::close_scope() {
    if(scopes.size() == 1)
        Logger::error(input, 0, 0, 1, "cannot pop the universe scope");
    scopes.pop_back();
}

void SymbolTable::print() {
	for (int i = scopes.size() - 1; i >= 0; i--)
		print_scope(i);
}

void SymbolTable::print_scope(int i) {
	std::cout << "Scope " << i << ":" << std::endl;
	for(auto record : scopes.at(i))
		std::cout << "[" << std::setw(10) << std::left << record.first <<"] " << record.second << std::endl;
}
