#include <iostream>
#include <string>

#include "code_gen.h"

int Label::counter = 0;
int Global::counter = 0;
int StrGlobal::counter = 0;

std::map<std::string, std::string> globals = {};
std::map<std::string, std::string> strings = {};

void emit(std::string line) {
	std::cout << line << std::endl;
}

void gen_pass_0(AST *ast) {
	if (ast->type == "program") {
		for (auto child: ast->children) {
			gen_pass_0(child);
		}
	} else if (ast->type == "globalvar") {
		if (ast->get_child(1)->attr == "string") {
			auto global = Global();
			auto str_global = StrGlobal();
			emit("    .data");
			emit(global.to_string() + ":");
			emit("    .word " + str_global.to_string());
			emit("    .text");
			globals[ast->get_child(0)->attr] = global.to_string();
			strings[str_global.to_string()] = "";
		} else {
			auto global = Global();
			emit("    .data");
			emit(global.to_string() + ":");
			emit("    .word 0");
			emit("    .text");
			globals[ast->get_child(0)->attr] = global.to_string();
		}
	}
}

void gen_pass_1(AST *ast, std::map<std::string, std::string> locals = {}) {
	if (ast->type == "program") {
		for (auto child: ast->children) {
			gen_pass_1(child);
		}
	}
	if (ast->type == "func") {
		emit(ast->get_child(0)->attr + ":");
		gen_pass_1(ast->get_child(2));
	} else if (ast->type == "funccall") {
		emit("TODO FUNCCALL");
	} else if (ast->type == "var") {
		emit("TODO VAR");
	} else if (ast->type == "block") {
		for (auto child: ast->children) {
			gen_pass_1(child);
		}
	} else if (ast->type == "if") {
		emit("TODO IF");
	} else if (ast->type == "for") {
		emit("TODO FOR");
	} else if (ast->type == "break") {
		emit("TODO BREAK");
	} else if (ast->type == "return") {
		emit("TODO RETURN");
	} else if (ast->type == "int") {
		emit("    li $t0," + ast->attr);
	} else if (ast->type == "string") {
		auto str_global = StrGlobal();
		emit("    la $t0," + str_global.to_string());
		strings[str_global.to_string()] = ast->attr;
	} else if (ast->type == "id") {
		if (ast->attr == "true") {
			emit("    li $t0,Ltrue");
		} else if (ast->attr == "false") {
			emit("    li $t0,Lfalse");
		} else {
			emit("TODO ID");
		}
	} else if (ast->type == "&&") {
		emit("TODO AND");
	} else if (ast->type == "||") {
		emit("TODO OR");
	} else if (ast->type == "==") {
		emit("TODO EQUALS EQUALS");
	} else if (ast->type == "!=") {
		emit("TODO NOT EQUALS");
	} else if (ast->type == ">=") {
		emit("TODO GTOE");
	} else if (ast->type == ">") {
		emit("TODO GT");
	} else if (ast->type == "<=") {
		emit("TODO LTOE");
	} else if (ast->type == "<") {
		emit("TODO LT");
	} else if (ast->type == "*") {
		emit("TODO MUL");
	} else if (ast->type == "/") {
		emit("TODO DIV");
	} else if (ast->type == "%") {
		emit("TODO MOD");
	} else if (ast->type == "+") {
		emit("TODO ADD");
	}else if (ast->type == "-") {
		gen_pass_1(ast->get_child(0));
		gen_pass_1(ast->get_child(1));
		emit("    subu $t4,$t1,$t3");
	} else if (ast->type == "!") {
		emit("TODO NOT");
	} else if (ast->type == "=") {
		gen_pass_1(ast->get_child(1));

		if (locals.contains(ast->get_child(0)->attr)) {
			emit("    sw $t0," + locals[ast->get_child(0)->attr] + "($sp)");
		} else {
			emit("    sw $t0," + globals[ast->get_child(0)->attr]);
		}
//		emit("TODO ASSIGN");
	}
}

void gen_pass_2() {
	if (strings.empty()) {
		return;
	}

	emit("    .data");
	for (auto &[label, value]: strings) {
		emit(label + ":");
		for (char &c: value) {
			emit("    .byte " + std::to_string(int(c)));
		}
		emit("    .byte 0");
	}
	emit("    .align 2");
	emit("    .text");
}

void generate_code(AST *root) {
	emit("    Ltrue = 1");
	emit("    Lfalse = 0");
	emit("    .text");
	emit("    .globl _start");
	emit("_start:");
	emit("    jal main");
	emit("    j Lhalt");
	emit("Lhalt:");
	emit("    li $v0,10");
	emit("    syscall");
	emit("    jr $ra");

	gen_pass_0(root);
	gen_pass_1(root);
	gen_pass_2();
//	root->pre([](auto ast) {
//		gen_pass1(ast, false);
//	});
}