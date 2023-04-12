#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "code_gen.h"

int Label::counter = 0;
int Global::counter = 0;
int StrGlobal::counter = 0;

std::string current_func;
int current_offset;

std::map<void*, std::string> vars = {};
std::map<std::string, std::string> strings = {
		{StrGlobal().to_string(), ""}
};

std::vector<std::string> all_registers {
		"$t9",
		"$t8",
		"$t7",
		"$t6",
		"$t5",
		"$t4",
		"$t3",
		"$t2",
		"$t1",
		"$t0",
};
std::map<std::string, std::vector<std::string>> available_registers {};

void populate_registers(std::string func){
	for (auto r : all_registers) {
		available_registers[func].push_back(r);
	}
}

std::string alloc_reg(){
	if (available_registers.find(current_func) == available_registers.end()) {
		populate_registers(current_func);
	}

	if (!available_registers[current_func].empty()) {
		std::string available_reg = available_registers[current_func].back();
		available_registers[current_func].pop_back();
		return available_reg;
	}

	else {
		std::cout << "error: not enough free registers" << std::endl;
		exit(1);
	}
}

void freereg(std::string reg){
	if (reg != "" && reg.substr(0, 2) != "$v" && reg.substr(0, 2) != "$a") {
		// cout << "    # Deallocated reg: " << reg << endl;
		available_registers[current_func].push_back(reg);
	}
}

std::ofstream source_file;
void emit(std::string line) {
	std::cout << line << std::endl;
	source_file << line << std::endl;
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
		current_func = ast->get_child(0)->attr;

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
		// Pass parameters
		int i = 0;
		for(auto actual : ast->get_child(1)->children) {
			gen_pass_1(actual);
			i++;
		}
		i = 0;
		for(auto actual : ast->get_child(1)->children) {
			emit("    move $a" + std::to_string(i) + "," + actual->reg);
			freereg(actual->reg);
			i++;
		}
		emit("    jal " + ast->get_child(0)->attr);
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
		auto start = Label();
		auto end = Label();

		// Start of loop
		emit(start.to_string() + ":");

		// Condition
		gen_pass_1(ast->get_child(0));
		emit("    beqz " + ast->get_child(0)->reg + "," + end.to_string());
		freereg(ast->get_child(0)->reg);

		// Body
		gen_pass_1(ast->get_child(1));
		emit("    j " + start.to_string());

		// End of loop
		emit(end.to_string() + ":");
	}

	else if (ast->type == "break") {
		emit("TODO BREAK");
	}

	else if (ast->type == "return") {
		emit("TODO RETURN");
	}

	else if (ast->type == "int") {
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    li " + reg + "," + ast->attr);
	}

	else if (ast->type == "string") {
		auto reg = alloc_reg();
		ast->reg = reg;
		auto str_global = StrGlobal();
		emit("    la " + reg + "," + str_global.to_string());
		strings[str_global.to_string()] = ast->attr;
	}

	else if (ast->type == "id") {
		auto reg = alloc_reg();
		ast->reg = reg;
		if (ast->attr == "true") {
			emit("    li " + reg + ",Ltrue");
		} else if (ast->attr == "false") {
			emit("    li " + reg + ",Lfalse");
		} else {
			emit("    lw " + reg + "," + vars[ast->sym]);
		}
	}

	else if (ast->type == "&&") {
		gen_pass_1(ast->get_child(0));
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    move " + reg + "," + ast->get_child(0)->reg);
		auto skip = Label();
		emit("    beqz " + reg + "," + skip.to_string());
		freereg(ast->get_child(0)->reg);
		gen_pass_1(ast->get_child(1));
		emit("    move " + reg + "," + ast->get_child(0)->reg);
		freereg(ast->get_child(1)->reg);
		emit(skip.to_string() + ":");
	}

	else if (ast->type == "||") {
		gen_pass_1(ast->get_child(0));
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    move " + reg + "," + ast->get_child(0)->reg);
		auto skip = Label();
		emit("    bnez " + reg + "," + skip.to_string());
		freereg(ast->get_child(0)->reg);
		gen_pass_1(ast->get_child(1));
		emit("    move " + reg + "," + ast->get_child(0)->reg);
		freereg(ast->get_child(1)->reg);
		emit(skip.to_string() + ":");
	}

	else if (ast->type == "==") {
		gen_pass_1(ast->get_child(0));
		gen_pass_1(ast->get_child(1));
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    seq " + reg + "," + ast->get_child(0)->reg + "," + ast->get_child(1)->reg);
		freereg(ast->get_child(1)->reg);
		freereg(ast->get_child(0)->reg);
	}

	else if (ast->type == "!=") {
		gen_pass_1(ast->get_child(0));
		gen_pass_1(ast->get_child(1));
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    sne " + reg + "," + ast->get_child(0)->reg + "," + ast->get_child(1)->reg);
		freereg(ast->get_child(1)->reg);
		freereg(ast->get_child(0)->reg);
	}

	else if (ast->type == ">=") {
		gen_pass_1(ast->get_child(0));
		gen_pass_1(ast->get_child(1));
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    sge " + reg + "," + ast->get_child(0)->reg + "," + ast->get_child(1)->reg);
		freereg(ast->get_child(1)->reg);
		freereg(ast->get_child(0)->reg);
	}

	else if (ast->type == ">") {
		gen_pass_1(ast->get_child(0));
		gen_pass_1(ast->get_child(1));
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    sgt " + reg + "," + ast->get_child(0)->reg + "," + ast->get_child(1)->reg);
		freereg(ast->get_child(1)->reg);
		freereg(ast->get_child(0)->reg);
	}

	else if (ast->type == "<=") {
		gen_pass_1(ast->get_child(0));
		gen_pass_1(ast->get_child(1));
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    sle " + reg + "," + ast->get_child(0)->reg + "," + ast->get_child(1)->reg);
		freereg(ast->get_child(1)->reg);
		freereg(ast->get_child(0)->reg);
	}

	else if (ast->type == "<") {
		gen_pass_1(ast->get_child(0));
		gen_pass_1(ast->get_child(1));
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    slt " + reg + "," + ast->get_child(0)->reg + "," + ast->get_child(1)->reg);
		freereg(ast->get_child(1)->reg);
		freereg(ast->get_child(0)->reg);
	}

	else if (ast->type == "*") {
		gen_pass_1(ast->get_child(0));
		gen_pass_1(ast->get_child(1));
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    mul " + reg + "," + ast->get_child(0)->reg + "," + ast->get_child(1)->reg);
		freereg(ast->get_child(1)->reg);
		freereg(ast->get_child(0)->reg);
	}

	else if (ast->type == "/") {
		gen_pass_1(ast->get_child(0));
		gen_pass_1(ast->get_child(1));
		emit("    move $a0," + ast->get_child(0)->reg);
		emit("    move $a1," + ast->get_child(1)->reg);
		emit("    jal divmodchk");
		emit("    move " + ast->get_child(1)->reg + ",$v0");
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    div " + reg + "," + ast->get_child(0)->reg + "," + ast->get_child(1)->reg);
		freereg(ast->get_child(1)->reg);
		freereg(ast->get_child(0)->reg);
	}

	else if (ast->type == "%") {
		gen_pass_1(ast->get_child(0));
		gen_pass_1(ast->get_child(1));
		emit("    move $a0," + ast->get_child(0)->reg);
		emit("    move $a1," + ast->get_child(1)->reg);
		emit("    jal divmodchk");
		emit("    move " + ast->get_child(1)->reg + ",$v0");
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    rem " + reg + "," + ast->get_child(0)->reg + "," + ast->get_child(1)->reg);
		freereg(ast->get_child(1)->reg);
		freereg(ast->get_child(0)->reg);
	}

	else if (ast->type == "+") {
		gen_pass_1(ast->get_child(0));
		gen_pass_1(ast->get_child(1));
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    addu " + reg + "," + ast->get_child(0)->reg + "," + ast->get_child(1)->reg);
		freereg(ast->get_child(1)->reg);
		freereg(ast->get_child(0)->reg);
	}

	else if (ast->type == "-") {
		gen_pass_1(ast->get_child(0));
		gen_pass_1(ast->get_child(1));
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    subu " + reg + "," + ast->get_child(0)->reg + "," + ast->get_child(1)->reg);
		freereg(ast->get_child(1)->reg);
		freereg(ast->get_child(0)->reg);
	}

	else if (ast->type == "!") {
		gen_pass_1(ast->get_child(0));
		freereg(ast->get_child(0)->reg);
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    xori " + reg + "," + ast->get_child(0)->reg + ",1");
	}

	else if (ast->type == "=") {
		gen_pass_1(ast->get_child(1));
		emit("    sw " + ast->get_child(1)->reg + "," + vars[ast->get_child(0)->sym]);
		freereg		(ast->get_child(1)->reg);
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
	source_file.open("a.out", std::ofstream::out | std::ofstream::trunc);

	emit("    Ltrue = 1");
	emit("    Lfalse = 0");
	emit("    .text");
	emit("    .globl _start");
	emit("_start:");
	emit("    jal main");
	emit("    j Lhalt");

	get_char();
	prints();
	printi();
	halt();
	printb();
	printc();

	gen_pass_0(root);
	gen_pass_1(root);
	gen_pass_2();

	source_file.close();
}

void get_char(){
	emit(".data");
	emit("char: .space 2");
	emit(".text");

	emit("Lgetchar: ");
	emit("  addi $sp,$sp,-4");
	emit("  sw $s0,0($sp)");
	emit("  li $v0,8");
	emit("  la $a0,char");
	emit("  la $a1,2");
	emit("  syscall");
	emit("  lb $v0,char");
	emit("  li $s0,4");
	emit("  beq $v0,$s0,Lgetchar_eof");
	emit("  li $s0,0");
	emit("  beq $v0,$s0,Lgetchar_eof");
	emit("Lgetchar_return:");
	emit("  lw $s0 0($sp)");
	emit("  addi $sp,$sp,4");
	emit("  jr $ra ");
	emit("Lgetchar_eof:");
	emit("  li $v0,-1");
	emit("  j Lgetchar_return");
}

void prints(){
	emit("Lprints: ");
	emit("  li $v0,4");
	emit("  syscall");
	emit("  jr $ra ");
}

void printi(){
	emit("Lprinti: ");
	emit("    li $v0,1");
	emit("    syscall");
	emit("    jr $ra ");

}

void halt(){
	emit("Lhalt: ");
	emit("    li $v0,10");
	emit("    syscall");
	emit("    jr $ra ");

}

void printb(){
	emit("Lprintb: ");
	emit("  li $v0,1");
	emit("  syscall");
	emit("  jr $ra ");

}

void printc(){
	emit("printc: ");
	emit("  li $v0,11");
	emit("  syscall");
	emit("  jr $ra ");
}