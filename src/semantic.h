#pragma once

#include <vector>
#include <map>
#include <string>

#include "input.h"
#include "ast.h"
#include "symbol_table.h"

/**
 * An identifier is redefined within the same scope.
 * An undeclared identifier is used.
 * A type was needed but something other than a type was used.
 * A type was used in a context where a type didn't belong.
 * An if-, if-else, or for-condition must be of Boolean type.
 * Type mismatch for an operator (||, &&, ==, !=, =, <, >, <=, >=, +, - (unary and binary), *, /, %, !).
 * Calling something that isn't a function.
 * The number/type of arguments in a function call doesn't match the function's declaration.
 * No main declaration found.
 * Multiple main declarations found.
 * The main function can't have a return value.
 * The left-hand side of an assignment may only be a variable.
 * A constant may not be assigned to.
 * break statements must be inside a for statement.
 * A return statement in a void function (i.e., one without a declared result type) can't return a value.
 * A return statement in a non-void function must return a value.
 * A value returned from a non-void function has the wrong type.
 * An integer literal is out of range.
 * No return statements at all are present in a non-void function.
 *  - Note that you're only checking for the existence of an appropriate return statement at the semantic checking stage, not whether it's actually executed.
 */

/**
 * What is the goal of semantic analysis?
 * We want to annotate and validate the ast
 * We can use a symbol table to help us track declarations and types
 */

class Semantic {
public:
    Semantic(Input *input, AST ast);
    AST analyze(bool verbose);

private:
    Input *input;
    AST ast;
    SymbolTable symbol_table;

    void check_identifiers();
    void tag_nodes();
    void check_main();
    void check_int_literal();
    void check_constant_assignment();
    void check_break_staktement();

    void pass_0();
    void pass_1();
    void pass_2();
    void pass_3();
    void pass_4();
};
