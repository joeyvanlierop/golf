#include <iostream>
#include "semantic.h"
#include "logger.h"

Semantic::Semantic(Input *input, AST ast) : input(input), ast(ast), symbol_table(input) {}

/**
 * TODO: Remove
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
                if (ast->type == "func")
                    symbol_table.define(ast->get_child(0)->attr,
                                        ast->get_child(1)->get_child(1)->type,
                                        ast->get_child(0)->line,
                                        ast->get_child(0)->column);
                if (ast->type == "=")
                    symbol_table.lookup(ast->get_child(0)->attr,
                                        ast->get_child(0)->line,
                                        ast->get_child(0)->column);
                if (ast->type == "block")
                    symbol_table.open_scope();
            },
            [this](auto ast) {
                if (ast->type == "block") {
                    symbol_table.close_scope();
                }
            });
}

/**
 * TODO: Remove
 * Semantic error: an identifier is redefined within the same scope
 * Semantic error: an undeclared identifier is used
 */
void Semantic::tag_nodes() {
    ast.pre_post(
            [this](auto ast) {
                if (ast->type == "id")
                    symbol_table.define(ast->get_child(0)->attr,
                                        ast->get_child(1)->type,
                                        ast->get_child(0)->line,
                                        ast->get_child(0)->column);
                if (ast->type == "=")
                    symbol_table.lookup(ast->get_child(0)->attr,
                                        ast->get_child(0)->line,
                                        ast->get_child(0)->column);
                if (ast->type == "block")
                    symbol_table.open_scope();
            },
            [this](auto ast) {
                if (ast->type == "block") {
                    symbol_table.close_scope();
                }
            });
}

/**
 * TODO: Remove
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
 * TODO: Remove
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

/**
 * TODO: Remove
 * Semantic error: a constant may not be assigned to
 */
void Semantic::check_constant_assignment() {
    ast.pre(
            [this](auto ast) {
                if (ast->type == "=") {
                    std::cout << ast->get_child(0)->type << std::endl;
                    if (ast->get_child(0)->type == "int")
                        Logger::error(input, ast->get_child(0)->line, ast->get_child(0)->column,
                                      ast->get_child(0)->attr.length(), "constant value cannot be assigned to");
                    if (ast->get_child(0)->type == "id" && (ast->get_child(0)->attr == "true" || ast->get_child(0)->attr == "false"))
                        Logger::error(input, ast->get_child(0)->line, ast->get_child(0)->column,
                                      ast->get_child(0)->attr.length(), "constant value cannot be assigned to");
                }
            });
}

/**
 * Pass 0:
 *
 * Populates the universe scope
 */
void Semantic::pass_0() { }

/**
 * Pass 1 (Pre traversal)
 *
 * Inserting the names of global variables and functions into the symbol table
 *   Not able to insert any information regarding the types
 *   Placeholders ("this name is declared, but not going to tell you what it is")
 *   Helps with forward declarations
 */
void Semantic::pass_1() { }

/**
 * Pass 2 (Pre-post traversal)
 *
 * Finish up what pass 1 started
 * Fully define all global names
 *   What names were inadvisable redefined by the user
 * Open/close scope when entering/exiting block
 * Annotate identifiers with pointer to record
 */
void Semantic::pass_2() { }

/**
 * Pass 3: (Post traversal)
 *
 * Does the majority of the type checking
 * Check operators and how they can be combined
 *   Table-driven to avoid linguini code
 * Check function calls
 *   Validate argument count
 *   Validate argument types
 */
void Semantic::pass_3() { }

/**
 * Pass 4: (Pre-post traversal)
 *
 * Check assignments
 *   Cannot assign to constants
 * Check if break statement is inside a for loop
 * Check if return statement is appropriate
 * Validate the main function
 *   Exactly one declaration
 *   No arguments
 *   No return type
 */
void Semantic::pass_4() { }

/**
 * Validates and annotates the given abstract syntax tree
 * @param verbose should the annotated abstract syntax tree be printed
 * @return the annotated abstract syntax tree (AST)
 */
AST Semantic::analyze(bool verbose) {
    // Perform the syntax analysis passes
    pass_0();
    pass_1();
    pass_2();
    pass_3();
    pass_4();

    // Print newly annotated ast
    if (verbose)
        ast.print();

    // Return the annotated ast
    return ast;
}
