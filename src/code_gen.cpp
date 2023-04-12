#include <iostream>
#include <string>

#include "code_gen.h"

int Label::counter = 0;
int Global::counter = 0;
int StrGlobal::counter = 0;

int current_offset;

std::map<void*, std::string> vars = {};
std::map<std::string, std::string> strings = {
		{StrGlobal().to_string(), ""}
};

void emit(std::string line) {
	std::cout << line << std::endl;
}

void gen_pass_0(AST *ast) {
	if (ast->type == "program") {
		for (auto child: ast->children) {
			gen_pass_0(child);
		}
	} else if (ast->type == "globalvar") {
		auto global = Global();
		emit("    .data");
		emit(global.to_string() + ":");
		if (ast->get_child(1)->attr == "string") {
			emit("    .word S0");
		} else {
			emit("    .word 0");
		}
		emit("    .text");
		vars[ast->sym] = global.to_string();
	}
}

void gen_pass_1(AST *ast) {
	if (ast->type == "program") {
		for (auto child: ast->children) {
			gen_pass_1(child);
		}
	}

	else if (ast->type == "func") {
		// Setup stack frame
		emit(ast->get_child(0)->attr + ":");
		auto locals = count_locals(ast->get_child(2));
		auto formals = ast->get_child(1)->get_child(0)->children.size();
		auto frame_size = (locals + formals) * 4 + 4;
		emit("    subu $sp,$sp," + std::to_string(frame_size));
		emit("    sw $ra,0($sp)");

		// Store parameters
		int i = 0;
		for(auto formal : ast->get_child(1)->get_child(0)->children) {
			auto offset = i * 4 + 4;
			emit("    sw $a" + std::to_string(i) + "," + std::to_string(offset) + "($sp)");
			vars[formal->get_child(0)->sym] = std::to_string(offset) + "($sp)";
			i++;
		}

		// Body
		current_offset = 4;
		gen_pass_1(ast->get_child(2));

		// Epilogue
		emit(ast->get_child(0)->attr + "_epilogue:");
		emit("    lw $ra,0($sp)");
		emit("    addu $sp,$sp," + std::to_string(frame_size));
		emit("    jr $ra");
	}

	else if (ast->type == "funccall") {
		emit("TODO FUNCCALL");
	}

	else if (ast->type == "var") {
		if(ast->get_child(1)->attr == "string") {
			emit("    la $v1,S0");
			emit("    sw $v1," + std::to_string(current_offset) + "($sp)");
		} else {
			emit("    sw $0," + std::to_string(current_offset) + "($sp)");
		}
		vars[ast->sym] = std::to_string(current_offset) + "($sp)";
		current_offset += 4;
	}

	else if (ast->type == "block") {
		for (auto child: ast->children) {
			gen_pass_1(child);
		}
	}

	else if (ast->type == "if") {
		emit("TODO IF");
	}

	else if (ast->type == "for") {
		emit("TODO FOR");
	}

	else if (ast->type == "break") {
		emit("TODO BREAK");
	}

	else if (ast->type == "return") {
		emit("TODO RETURN");
	}

	else if (ast->type == "int") {
		emit("    li $t0," + ast->attr);
	}

	else if (ast->type == "string") {
		auto str_global = StrGlobal();
		emit("    la $t0," + str_global.to_string());
		strings[str_global.to_string()] = ast->attr;
	}

	else if (ast->type == "id") {
		if (ast->attr == "true") {
			emit("    li $t0,Ltrue");
		} else if (ast->attr == "false") {
			emit("    li $t0,Lfalse");
		} else {
			emit("    lw $t0," + vars[ast->sym]);
		}
	}

	else if (ast->type == "&&") {
		emit("TODO AND");
	}

	else if (ast->type == "||") {
		emit("TODO OR");
	}

	else if (ast->type == "==") {
		emit("TODO EQUALS EQUALS");
	}

	else if (ast->type == "!=") {
		emit("TODO NOT EQUALS");
	}

	else if (ast->type == ">=") {
		emit("TODO GTOE");
	}

	else if (ast->type == ">") {
		emit("TODO GT");
	}

	else if (ast->type == "<=") {
		emit("TODO LTOE");
	}

	else if (ast->type == "<") {
		emit("TODO LT");
	}

	else if (ast->type == "*") {
		emit("TODO MUL");
	}

	else if (ast->type == "/") {
		emit("TODO DIV");
	}

	else if (ast->type == "%") {
		emit("TODO MOD");
	}

	else if (ast->type == "+") {
		emit("TODO ADD");
	}

	else if (ast->type == "-") {
		gen_pass_1(ast->get_child(0));
		gen_pass_1(ast->get_child(1));
		emit("    subu $t4,$t1,$t3");
	}

	else if (ast->type == "!") {
		emit("TODO NOT");
	}

	else if (ast->type == "=") {
		gen_pass_1(ast->get_child(1));
		emit("    sw $t0," + vars[ast->get_child(0)->sym]);
//		emit("TODO ASSIGN");
	}
}

int count_locals(AST *ast) {
	int count = 0;
	if (ast->type == "var") {
		count++;
	} else if (ast->type == "block") {
		for (auto child: ast->children) {
			count += count_locals(child);
		}
	} else if (ast->type == "if") {
		count += count_locals(ast->get_child(1));
		if (ast->children.size() == 3) {
			count += count_locals(ast->get_child(2)->get_child(0));
		}
	} else if (ast->type == "for") {
		count += count_locals(ast->get_child(1));
	}
	return count;
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
	emit("Lhalt:");
	emit("    li $v0,10");
	emit("    syscall");
	emit("    jr $ra");
	emit("_start:");
	emit("    jal main");
	emit("    j Lhalt");

	gen_pass_0(root);
	gen_pass_1(root);
	gen_pass_2();
//	root->pre([](auto ast) {
//		gen_pass1(ast, false);
//	});
}