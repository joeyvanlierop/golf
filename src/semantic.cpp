#include <iostream>

#include "semantic.h"
#include "logger.h"

Semantic::Semantic(Input *input, AST ast) : input(input), ast(ast), symbol_table(input) {}

/**
 * Pass 0:
 *
 * Populates the universe scope
 */
void Semantic::pass_0() {
	// Open the global scope
	// We dont really need to close this
	symbol_table.open_scope();

	// Add all of our
	symbol_table.define("$void", {"void", "", false, true});
	symbol_table.define("bool", {"bool", "", false, true});
	symbol_table.define("int", {"int", "", false, true});
	symbol_table.define("string", {"str", "", false, true});
	symbol_table.define("$true", {"bool", "", true, false});
	symbol_table.define("true", {"bool", "", true, false});
	symbol_table.define("false", {"bool", "", true, false});
	symbol_table.define("printb", {"f(bool)", "void", false, false});
	symbol_table.define("printc", {"f(int)", "void", false, false});
	symbol_table.define("printi", {"f(int)", "void", false, false});
	symbol_table.define("prints", {"f(str)", "void", false, false});
	symbol_table.define("getchar", {"f()", "void", false, false});
	symbol_table.define("halt", {"f()", "void", false, false});
	symbol_table.define("len", {"f(str)", "int", false, false});
}

/**
 * Pass 1 (Pre traversal)
 *
 * Populates the global scope
 * Inserting the names of global variables and functions into the symbol table
 *   Not able to insert any information regarding the types
 *   Placeholders ("this name is declared, but not going to tell you what it is")
 *   Helps with forward declarations
 */
void Semantic::pass_1() {
	// Open the global scope
	// We dont really need to close this
	symbol_table.open_scope();

	// Traverse the global identifiers
	ast.pre([this](auto ast) {
		if (ast->type == "globalvar") {
			symbol_table.define(ast->get_child(0), {});
		} else if (ast->type == "func") {
			symbol_table.define(ast->get_child(0), {});
		}
	});
}

/**
 * Pass 2 (Pre-post traversal)
 *
 * Populates the local scopes
 * Finish up what pass 1 started
 * Fully define all global names
 *   What names were inadvisable redefined by the user
 * Open/close scope when entering/exiting block
 * Annotate identifiers with pointer to record
 */
void Semantic::pass_2() {
	ast.pre_post([this](auto ast) {
					 if (ast->type == "globalvar") {
						 auto var_decl = symbol_table.lookup(ast->get_child(0));
						 auto type = symbol_table.lookup(ast->get_child(1));
						 var_decl->sig = type->sig;
						 var_decl->is_const = false;
						 var_decl->is_type = false;
						 ast->sym = var_decl;
					 } else if (ast->type == "func") {
						 auto func_decl = symbol_table.lookup(ast->get_child(0));
						 auto sig = encode_func_decl(ast);
						 func_decl->sig = sig;
						 auto rt_sig = symbol_table.lookup(ast->get_child(1)->get_child(1));
						 func_decl->rt_sig = rt_sig->sig;
						 func_decl->is_const = false;
						 func_decl->is_type = false;
						 ast->sym = func_decl;
					 } else if (ast->type == "var") {
						 auto name = ast->get_child(0)->attr;
						 auto type = symbol_table.lookup(ast->get_child(1));
						 ast->sym = symbol_table.define(name, {type->sig, "", false, false});
					 } else if (ast->type == "typeid") {
						 auto type = symbol_table.lookup(ast);
						 if (!type->is_type)
							 Logger::error(input, ast->line, ast->column, ast->attr.length(), "expected type");
						 ast->sym = type;
					 } else if (ast->type == "id") {
						 auto var_decl = symbol_table.lookup(ast);
						 ast->sym = var_decl;
						 ast->sig = var_decl->sig;
					 } else if (ast->type == "int") {
						 auto type = symbol_table.lookup("int");
						 ast->sig = type->sig;
					 } else if (ast->type == "string") {
						 auto type = symbol_table.lookup("string");
						 ast->sig = type->sig;
					 } else if (ast->type == "bool") {
						 auto bool_decl = symbol_table.lookup(ast->attr);
						 auto type = symbol_table.lookup("bool");
						 ast->sym = bool_decl;
						 ast->sig = type->sig;
					 } else if (ast->type == "block") {
						 symbol_table.open_scope();
					 }
				 },
				 [this](auto ast) {
					 if (ast->type == "block") {
						 symbol_table.close_scope();
					 }
				 });
}

std::string Semantic::encode_func_decl(AST *ast) {
	std::string sig = "f(";
	for (int i = 0; i < ast->get_child(1)->get_child(0)->children.size(); i++) {
		sig.append(ast->get_child(1)->get_child(0)->get_child(i)->get_child(1)->attr);
		if (i < ast->get_child(1)->get_child(0)->children.size() - 1)
			sig.append(",");
	}
	sig.append(")");
	return sig;
}

/**
 * Pass 3: (Post traversal)
 *
 * Does the majority of the type checking
 * Check operators and how they can be combined
 *   Table-driven to avoid linguini code
 * Check function calls
 *   Validate argument count
 *   Validate argument types
 * No need to use reconstruct the symbol table since we annotated the ast
 */
void Semantic::pass_3() {
	ast.post([this](auto ast) {
		if (ast->children.size() == 2 && legal_binary.count(ast->type)) {
			ast->sig = check_binary(ast);
		} else if (ast->children.size() == 1 && legal_unary.count(ast->type)) {
			ast->sig = check_unary(ast);
		} else if (ast->type == "funccall") {
			auto func_call = encode_func_call(ast);
			auto func_decl = symbol_table.lookup(ast->get_child(0)->attr);
			if (func_call != func_decl->sig)
				Logger::error(input, ast->line, ast->column, ast->attr.length(),
							  "function call does not match function signature");
			ast->sig = func_decl->rt_sig;
		}
	});
}

std::string Semantic::check_binary(AST *ast) {
	auto op = ast->type;
	auto iter = legal_binary.find(op);
	auto left = ast->get_child(0);
	auto right = ast->get_child(1);
	const auto &legal_types = iter->second;
	for (const auto &t: legal_types)
		if (std::get<0>(t) == left->sig && std::get<0>(t) == right->sig)
			return std::get<2>(t);

	Logger::error(input, right->line, right->column, right->attr.length(), "invalid operand");
	throw 0;
}

std::string Semantic::check_unary(AST *ast) {
	auto op = ast->type;
	auto iter = legal_unary.find(op);
	auto operand = ast->get_child(0);
	const auto &legal_types = iter->second;
	for (const auto &t: legal_types)
		if (std::get<0>(t) == operand->sig)
			return std::get<1>(t);

	Logger::error(input, operand->line, operand->column, operand->attr.length(), "invalid operand");
	throw 0;
}

std::string Semantic::encode_func_call(AST *ast) {
	std::string sig = "f(";
	for (int i = 0; i < ast->get_child(1)->children.size(); i++) {
		sig.append(ast->get_child(1)->get_child(i)->sig);
		if (i < ast->get_child(1)->children.size() - 1)
			sig.append(",");
	}
	sig.append(")");
	return sig;
}

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
void Semantic::pass_4() {
	auto for_depth = 0;
	auto main_count = 0;

	ast.pre_post([this, &for_depth, &main_count](auto ast) {

					 if (ast->type == "for") {
						 for_depth++;
					 } else if (ast->type == "break") {
						 if (for_depth == 0) {
							 Logger::error(input, ast->line, ast->column, ast->type.length(), "break must be inside a for loop");
						 }
					 } else if (ast->type == "func") {
						 if (ast->get_child(0)->attr == "main") {
							 auto main_record = symbol_table.lookup(ast->get_child(0));
							 if(main_record->sig != "f()")
								 Logger::error(input, ast->line, ast->column, ast->attr.length(), "main function cannot have arguments");
							 else if(main_record->rt_sig != "void")
								 Logger::error(input, ast->line, ast->column, ast->attr.length(), "main function cannot have a return type");
							 main_count++;
						 }
					 }
				 },
				 [&for_depth](auto ast) {
					 if (ast->type == "for") {
						 for_depth--;
					 }
				 });

	if(main_count == 0) {
		Logger::error(input, 1, 1, 1, "missing main function");
	}
}

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
