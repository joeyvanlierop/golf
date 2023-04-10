#include <iostream>

#include "semantic_1.h"
#include "logger.h"

SemanticPass1::SemanticPass1(Input* input, AST* ast) : input(input), ast(ast), symbol_table(input) {}

/**
 * Pass 1 (Pre traversal)
 *
 * Populates the global scope
 * Inserting the names of global variables and functions into the symbol table
 *   Not able to insert any information regarding the types
 *   Placeholders ("this name is declared, but not going to tell you what it is")
 *   Helps with forward declarations
 */
void SemanticPass1::run() {
	// Open the global scope
	// We dont really need to close this
	symbol_table.open_scope();

	ast->accept(*this);
}

//std::any SemanticPass1::visit(std::shared_ptr<Program> stmt) {
//	return std::any();
//}
//
//std::any SemanticPass1::visit(std::shared_ptr<Variable> stmt) {
//	return std::any();
//}
//
//// Traverse the global identifiers
//	ast.pre([this](auto ast) {
//		if (ast->type == "globalvar") {
//			symbol_table.define(ast->get_child(0), {});
//		} else if (ast->type == "func") {
//			symbol_table.define(ast->get_child(0), {});
//		}
//	});
//
//	// Populate the global identifiers
//	ast.pre([this](auto ast) {
//		if (ast->type == "globalvar") {
//			auto var_decl = symbol_table.lookup(ast->get_child(0));
//			auto type = symbol_table.lookup(ast->get_child(1));
//			if (!type->is_type)
//				Logger::error(input, ast->get_child(1)->line, ast->get_child(1)->column, ast->get_child(1)->attr.length(), "expected type");
//			var_decl->sig = type->sig;
//			var_decl->is_const = false;
//			var_decl->is_type = false;
//			ast->sym = var_decl;
//		} else if (ast->type == "func") {
//			auto func_decl = symbol_table.lookup(ast->get_child(0));
//			auto sig = encode_func_decl(ast);
//			func_decl->sig = sig;
//			auto rt_sig = symbol_table.lookup(ast->get_child(1)->get_child(1));
//			func_decl->rt_sig = rt_sig->sig;
//			func_decl->is_const = false;
//			func_decl->is_type = false;
//			ast->sym = func_decl;
//		}
//	});
//}

