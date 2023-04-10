#pragma once

#include <vector>
#include <map>
#include <set>
#include <string>

#include "input.h"
#include "ast.h"
#include "symbol_table.h"

class SemanticPass1 : ASTVisitor {
public:
	SemanticPass1(Input* input, AST* ast);

	void run();


private:
	Input* input;
	AST* ast;
	SymbolTable symbol_table;

	std::any visit(std::shared_ptr<Program> stmt) override;
	std::any visit(std::shared_ptr<Variable> stmt) override;
	std::any visit(std::shared_ptr<Id> stmt) override;
	std::any visit(std::shared_ptr<Type> stmt) override;
	std::any visit(std::shared_ptr<Function> stmt) override;
	std::any visit(std::shared_ptr<FunctionSignature> stmt) override;
	std::any visit(std::shared_ptr<Formal> stmt) override;
	std::any visit(std::shared_ptr<Block> stmt) override;
	std::any visit(std::shared_ptr<If> stmt) override;
	std::any visit(std::shared_ptr<Else> stmt) override;
	std::any visit(std::shared_ptr<For> stmt) override;
	std::any visit(std::shared_ptr<Break> stmt) override;
	std::any visit(std::shared_ptr<Return> stmt) override;
	std::any visit(std::shared_ptr<Assignment> stmt) override;
	std::any visit(std::shared_ptr<Binary> stmt) override;
	std::any visit(std::shared_ptr<Unary> stmt) override;
	std::any visit(std::shared_ptr<FunctionCall> stmt) override;
	std::any visit(std::shared_ptr<Literal> stmt) override;
	std::any visit(std::shared_ptr<Empty> stmt) override;
};
