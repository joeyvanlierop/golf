#pragma once

#include <string>
#include <map>

#include "ast.h"

class Label {
private:
	static int counter;
	int value;
public:
	Label() {
		value = counter++;
	}

	std::string to_string() {
		return "L" + std::to_string(value);
	}
};

class Global {
private:
	static int counter;
	int value;
public:
	Global() {
		value = counter++;
	}

	std::string to_string() {
		return "G" + std::to_string(value);
	}
};

class StrGlobal {
private:
	static int counter;
	int value;
public:
	StrGlobal() {
		value = counter++;
	}

	std::string to_string() {
		return "S" + std::to_string(value);
	}
};

void emit(std::string line);
void gen_pass_0(AST *ast);
void gen_pass_1(AST *ast, bool in_call);
void gen_pass_2();
int count_locals(AST *ast);
void generate_code(AST *root);

// Predefined functions
void get_char();
void prints();
void printi();
void halt();
void printb();
void printc();
void len();