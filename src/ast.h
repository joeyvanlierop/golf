#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <any>
#include "record.h"

struct Program;
struct Variable;
struct Id;
struct Type;
struct Function;
struct FunctionSignature;
struct Formal;
struct Block;
struct If;
struct Else;
struct For;
struct Break;
struct Return;
struct Assignment;
struct Binary;
struct Unary;
struct FunctionCall;
struct Literal;
struct Empty;

struct ASTVisitor {
	virtual std::any visit(std::shared_ptr<Program> stmt) = 0;
	virtual std::any visit(std::shared_ptr<Variable> stmt) = 0;
	virtual std::any visit(std::shared_ptr<Id> stmt) = 0;
	virtual std::any visit(std::shared_ptr<Type> stmt) = 0;
	virtual std::any visit(std::shared_ptr<Function> stmt) = 0;
	virtual std::any visit(std::shared_ptr<FunctionSignature> stmt) = 0;
	virtual std::any visit(std::shared_ptr<Formal> stmt) = 0;
	virtual std::any visit(std::shared_ptr<Block> stmt) = 0;
	virtual std::any visit(std::shared_ptr<class If> stmt) = 0;
	virtual std::any visit(std::shared_ptr<class Else> stmt) = 0;
	virtual std::any visit(std::shared_ptr<class For> stmt) = 0;
	virtual std::any visit(std::shared_ptr<class Break> stmt) = 0;
	virtual std::any visit(std::shared_ptr<class Return> stmt) = 0;
	virtual std::any visit(std::shared_ptr<Assignment> stmt) = 0;
	virtual std::any visit(std::shared_ptr<Binary> stmt) = 0;
	virtual std::any visit(std::shared_ptr<Unary> stmt) = 0;
	virtual std::any visit(std::shared_ptr<FunctionCall> stmt) = 0;
	virtual std::any visit(std::shared_ptr<Literal> stmt) = 0;
	virtual std::any visit(std::shared_ptr<Empty> stmt) = 0;
};

class AST {
public:
	int line;
	int column;
	Record *sym;

	AST(std::string type);

	AST(std::string type, std::string attr);

	AST(std::string type, std::string attr, int line, int column);

	AST(int line, int column);

	void print();

	virtual std::any accept(ASTVisitor& visitor) = 0;

private:
	void print(int indent);
};

struct Program : public AST, std::enable_shared_from_this<Program> {
	std::vector<std::shared_ptr<AST>> statements;

	Program(std::vector<std::shared_ptr<AST>> statements) :
			AST(-1, -1), statements(statements) {}

	std::any accept(ASTVisitor& visitor) override {
		return visitor.visit(shared_from_this());
	}
};

struct Variable : public AST, std::enable_shared_from_this<Variable> {
	std::shared_ptr<AST> id;
	std::shared_ptr<AST> type;

	Variable(int line, int column, std::shared_ptr<AST> id, std::shared_ptr<AST> type) :
			AST(line, column), id(id), type(type) {}

	std::any accept(ASTVisitor& visitor) override {
		return visitor.visit(shared_from_this());
	}
};

struct Id : public AST, std::enable_shared_from_this<Id> {
	std::string name;

	Id(int line, int column, std::string name) :
			AST(line, column), name(name) {}

	std::any accept(ASTVisitor& visitor) override {
		return visitor.visit(shared_from_this());
	}
};

struct Type : public AST, std::enable_shared_from_this<Type> {
	std::string name;

	Type(int line, int column, std::string name) :
			AST(line, column), name(name) {}

	std::any accept(ASTVisitor& visitor) override {
		return visitor.visit(shared_from_this());
	}

};

struct Function : public AST, std::enable_shared_from_this<Function> {
	std::shared_ptr<AST> id;
	std::shared_ptr<AST> sig;
	std::shared_ptr<AST> body;

	Function(int line, int column, std::shared_ptr<AST> id, std::shared_ptr<AST> body) :
			AST(line, column), id(id), sig(sig), body(body) {}

	std::any accept(ASTVisitor& visitor) override {
		return visitor.visit(shared_from_this());
	}
};

struct FunctionSignature : public AST, std::enable_shared_from_this<FunctionSignature> {
	std::vector<std::shared_ptr<AST>> formals;
	std::shared_ptr<AST> type;

	FunctionSignature(std::vector<std::shared_ptr<AST>> formals, std::shared_ptr<AST> type) :
	AST(-1, -1), formals(formals), type(type) {}

std::any accept(ASTVisitor& visitor) override {
		return visitor.visit(shared_from_this());
	}
};

struct Formal : public AST, std::enable_shared_from_this<Formal> {
	std::shared_ptr<AST> id;
	std::shared_ptr<AST> type;

	Formal(std::shared_ptr<AST> id, std::shared_ptr<AST> type) :
			AST(-1, -1), id(id), type(type) {}

	std::any accept(ASTVisitor& visitor) override {
		return visitor.visit(shared_from_this());
	}
};

struct Block : public AST, std::enable_shared_from_this<Block> {
	std::vector<std::shared_ptr<AST>> statements;

	Block(std::vector<std::shared_ptr<AST>> statements) :
			AST(-1, -1), statements(statements) {}

	std::any accept(ASTVisitor& visitor) override {
		return visitor.visit(shared_from_this());
	}
};

struct If : public AST, std::enable_shared_from_this<class If> {
	std::shared_ptr<AST> condition;
	std::shared_ptr<AST> body;
	std::shared_ptr<AST> branch;

	If(int line, int column, std::shared_ptr<AST> condition, std::shared_ptr<AST> body, std::shared_ptr<AST> branch) :
			AST(line, column), condition(condition), body(body), branch(branch) {}

	std::any accept(ASTVisitor& visitor) override {
		return visitor.visit(shared_from_this());
	}
};

struct Else : public AST, std::enable_shared_from_this<class Else> {
	std::shared_ptr<AST> body;

	Else(int line, int column, std::shared_ptr<AST> body) :
			AST(line, column), body(body) {}

	std::any accept(ASTVisitor& visitor) override {
		return visitor.visit(shared_from_this());
	}
};

struct For : public AST, std::enable_shared_from_this<class For> {
	std::shared_ptr<AST> condition;
	std::shared_ptr<AST> body;

	For(int line, int column, std::shared_ptr<AST> condition, std::shared_ptr<AST> body) :
			AST(line, column), condition(condition), body(body) {}

	std::any accept(ASTVisitor& visitor) override {
		return visitor.visit(shared_from_this());
	}
};

struct Break : public AST, std::enable_shared_from_this<class Break> {
	Break(int line, int column) :
			AST(line, column) {}

	std::any accept(ASTVisitor& visitor) override {
		return visitor.visit(shared_from_this());
	}
};

struct Return : public AST, std::enable_shared_from_this<class Return> {
	std::shared_ptr<AST> value;

	Return(int line, int column, std::shared_ptr<AST> value) :
			AST(line, column), value(value) {}

	std::any accept(ASTVisitor& visitor) override {
		return visitor.visit(shared_from_this());
	}
};

struct Assignment : public AST, std::enable_shared_from_this<Assignment> {
	std::shared_ptr<AST> left;
	std::shared_ptr<AST> right;

	Assignment(int line, int column, std::shared_ptr<AST> left, std::shared_ptr<AST> right) :
			AST(line, column), left(left), right(right) {}

	std::any accept(ASTVisitor& visitor) override {
		return visitor.visit(shared_from_this());
	}
};

struct Binary : public AST, std::enable_shared_from_this<Binary> {
	std::string op;
	std::shared_ptr<AST> left;
	std::shared_ptr<AST> right;

	Binary(int line, int column, std::string op, std::shared_ptr<AST> left, std::shared_ptr<AST> right) :
			AST(line, column), op(op), left(left), right(right) {}

	std::any accept(ASTVisitor& visitor) override {
		return visitor.visit(shared_from_this());
	}
};

struct Unary : public AST, std::enable_shared_from_this<Unary> {
	std::string op;
	std::shared_ptr<AST> operand;

	Unary(int line, int column, std::string op, std::shared_ptr<AST> operand) :
			AST(line, column), op(op), operand(operand) {}

	std::any accept(ASTVisitor& visitor) override {
		return visitor.visit(shared_from_this());
	}
};

struct FunctionCall : public AST, std::enable_shared_from_this<FunctionCall> {
	std::shared_ptr<AST> operand;
	std::vector<std::shared_ptr<AST>> actuals;

	FunctionCall(std::shared_ptr<AST> operand, std::vector<std::shared_ptr<AST>> actuals) :
		AST(-1, -1), operand(operand), actuals(actuals) {}

	std::any accept(ASTVisitor& visitor) override {
		return visitor.visit(shared_from_this());
	}
};

struct Literal : public AST, std::enable_shared_from_this<Literal> {
	enum Type { Int, String };

	Type type;
	std::string value;

	Literal(int line, int column, std::string value, Type type) :
			AST(line, column), value(value), type(type) {}

	std::any accept(ASTVisitor& visitor) override {
		return visitor.visit(shared_from_this());
	}
};

struct Empty : public AST, public std::enable_shared_from_this<Empty> {
	Empty() :
		AST(-1, -1) {}

	std::any accept(ASTVisitor& visitor) override {
		return visitor.visit(shared_from_this());
	}
};

