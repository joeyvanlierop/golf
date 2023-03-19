#include <iostream>
#include "semantic.h"
#include "logger.h"

Semantic::Semantic(Input *input, AST ast) : input(input), ast(ast), symbol_table(input) {}

/**
 * Semantic error: an identifier is redefined within the same scope
 * Semantic error: an undeclared identifier is used
 */
void Semantic::check_identifiers() {
    ast.pre_post(
            [this](auto ast) {
                if (ast->type == "var")
                    symbol_table.define(ast->get_child(0)->attr,
                                        ast->get_child(1)->type,
                                        ast->get_child(0)->line,
                                        ast->get_child(0)->column);
                if (ast->type == "=")
                    symbol_table.lookup(ast->get_child(0)->attr,
                                        ast->get_child(0)->line,
                                        ast->get_child(0)->column);
                if (ast->type == "block")
                    symbol_table.push_scope();
            },
            [this](auto ast) {
                if (ast->type == "block") {
                    symbol_table.pop_scope();
                }
            });
}

/**
 * Semantic error: no main declaration found
 * Semantic error: multiple main declarations found
 * Semantic error: the main function cannot have a return value
 */
void Semantic::check_main() {
    bool has_main = false;
    for (auto child: ast.children)
        if (child->type == "func")
            if (child->get_child(0)->attr == "main")
                if (has_main)
                    Logger::error(input, child->get_child(0)->line, child->get_child(0)->column,
                                  child->get_child(0)->attr.length(), "multiple main declarations found");
                else if (child->get_child(1)->get_child(1)->attr != "$void")
                    Logger::error(input, child->get_child(1)->get_child(1)->line,
                                  child->get_child(1)->get_child(1)->column,
                                  child->get_child(1)->get_child(1)->attr.length(),
                                  "main function cannot have a return value");
                else
                    has_main = true;
    if (!has_main)
        Logger::error(input, 1, 1, 1, "missing main function");
}

/**
 * Semantic error: an integer literal is out of range
 */
void Semantic::check_int_literal() {
    ast.pre(
            [this](auto ast) {
                if (ast->type == "int") {
                    std::cout << ast->attr << std::endl;
                    if (ast->attr.length() > 11)
                        Logger::error(input, ast->line, ast->column, ast->attr.length(),
                                      "integer literal out of range");
                    if (std::stoll(ast->attr) > 2147483647)
                        Logger::error(input, ast->line, ast->column, ast->attr.length(), "integer literal too large");
                    if (std::stoll(ast->attr) < -2147483648)
                        Logger::error(input, ast->line, ast->column, ast->attr.length(), "integer literal too small");
                }
            });
}

AST Semantic::analyze(bool verbose) {
    check_identifiers();
    check_main();
    check_int_literal();

    if (verbose) {
        ast.print();
    }
    return ast;
}
