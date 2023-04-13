#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

#include "code_gen.h"

/**
 * I'm sorry if you have to read this code
 */

int Label::counter = 0;
int Global::counter = 0;
int StrGlobal::counter = 0;

std::vector<std::string> break_stack;
std::string current_func;
int current_offset;

std::map<void*, std::string> vars = {};

auto empty_string = StrGlobal();
std::map<std::string, std::string> global_to_string = {
		{empty_string.to_string(), ""}
};
std::map<std::string, std::string> string_to_global = {
		{"", empty_string.to_string()}
};

std::vector<std::string> all_registers {
		"$s9",
		"$s8",
		"$s7",
		"$s6",
		"$s5",
		"$s4",
		"$s3",
		"$s2",
		"$s1",
		"$s0",
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
std::vector<std::string> used_registers {};

std::map<std::string, bool> redefined = {
		{"getchar", false},
		{"halt", false},
		{"len", false},
		{"printb", false},
		{"printc", false},
		{"printi", false},
		{"prints", false},
		{"divmodchk", false},
		{"error", false},
};


void populate_registers(std::string func){
	available_registers[func].clear();
	for (auto r : all_registers) {
		available_registers[func].push_back(r);
	}
}

std::string alloc_reg(){
	if (available_registers.find(current_func) == available_registers.end()) {
		populate_registers(current_func);
		used_registers.clear();
	}

	if (!available_registers[current_func].empty()) {
		std::string available_reg = available_registers[current_func].back();
		available_registers[current_func].pop_back();
		used_registers.push_back(available_reg);
		return available_reg;
	}

	else {
		std::cerr << "error: not enough free registers" << std::endl;
		exit(1);
	}
}

void freereg(std::string reg){
	if(std::find(available_registers[current_func].begin(), available_registers[current_func].end(), reg) != available_registers[current_func].end()) {
		return;
	}

	available_registers[current_func].push_back(reg);
	used_registers.erase(std::remove(used_registers.begin(), used_registers.end(), reg), used_registers.end());
}

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

void gen_pass_1(AST *ast, bool in_call = false) {
	if (ast->type == "program") {
		for (auto child: ast->children) {
			gen_pass_1(child);
		}
	}

	else if (ast->type == "func") {
		// Update current function
		current_func = ast->get_child(0)->attr;

		// Check if overwriting predefined function
		if(redefined.count(ast->get_child(0)->attr)) {
			redefined[ast->get_child(0)->attr] = true;
		}

		// Setup stack frame
		emit(ast->get_child(0)->attr + ":");
		auto locals = count_locals(ast->get_child(2));
		auto formals = ast->get_child(1)->get_child(0)->children.size();
		auto frame_size = (locals + formals) * 4 + 4;
		emit("    subu $sp,$sp," + std::to_string(frame_size));
		emit("    sw $ra,0($sp)");

		// Store parameters
		int i = 0;
		current_offset = 4;
		for(auto formal : ast->get_child(1)->get_child(0)->children) {
			auto offset = i * 4 + 4;
			emit("    sw $a" + std::to_string(i) + "," + std::to_string(current_offset) + "($sp)");
			vars[formal->get_child(0)->sym] = std::to_string(offset) + "($sp)";
			current_offset += 4;
			i++;
		}

		// Body
		gen_pass_1(ast->get_child(2));

		// Return validation
		if(ast->get_child(1)->get_child(1)->attr != "$void") {
			auto error_string = "error: function \'" + ast->get_child(0)->attr + "\' must return a value\n";
			auto error_string_global = StrGlobal();
			string_to_global[error_string] = error_string_global.to_string();
			global_to_string[error_string_global.to_string()] = error_string;
			emit("    la $a0," + error_string_global.to_string());
			emit("    j error");
		}

		// Epilogue
		emit(ast->get_child(0)->attr + "_epilogue:");
		emit("    lw $ra,0($sp)");
		emit("    addu $sp,$sp," + std::to_string(frame_size));
		emit("    jr $ra");
	}

	else if (ast->type == "funccall") {
		// Calculate parameters
		int i = 0;
		for(auto actual : ast->get_child(1)->children) {
			gen_pass_1(actual, true);
			i++;
		}
		for(auto actual : ast->get_child(1)->children) {
			freereg(actual->reg);
		}

		// Save registers
		auto saved_available = available_registers;
		auto saved = used_registers;
		if(in_call) {
			emit("    subu $sp,$sp," + std::to_string(saved.size() * 4));
			i = 0;
			for(auto reg : saved) {
				emit("    sw " + reg + "," + std::to_string(i * 4) + "($sp)");
				freereg(reg);
				i++;
			}
		}

		// Store parameters
		i = 0;
		for(auto actual : ast->get_child(1)->children) {
			emit("    move $a" + std::to_string(i) + "," + actual->reg);
			freereg(actual->reg);
			i++;
		}
		emit("    jal " + ast->get_child(0)->attr);

		// Load registers
		if(in_call) {
			i = 0;
			available_registers = saved_available;
			used_registers = saved;
			for (auto reg: saved) {
				emit("    lw " + reg + "," + std::to_string(i * 4) + "($sp)");
				i++;
			}
			emit("    addu $sp,$sp," + std::to_string(saved.size() * 4));
		}

		// Save output
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    move " + reg + ",$v0");

		// Free register if not in call (holy smokes this is hot garbage
		if(!in_call) {
			freereg(reg);
		}
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
		populate_registers(current_func);
		used_registers.clear();
	}

	else if (ast->type == "if") {
		auto elze = Label();
		auto end = Label();

		// Condition
		gen_pass_1(ast->get_child(0));
		if(ast->children.size() == 3) {
			emit("    beqz " + ast->get_child(0)->reg + "," + elze.to_string());
		} else {
			emit("    beqz " + ast->get_child(0)->reg + "," + end.to_string());
		}
		freereg(ast->get_child(0)->reg);

		// If body
		gen_pass_1(ast->get_child(1));
		emit("    j " + end.to_string());

		// Else body
		if(ast->children.size() == 3) {
			emit(elze.to_string() + ":");
			gen_pass_1(ast->get_child(2));
		}

		// End of loop
		emit(end.to_string() + ":");
	}

	else if (ast->type == "else") {
		// Condition
		gen_pass_1(ast->get_child(0));
	}

	else if (ast->type == "for") {
		auto start = Label();
		auto end = Label();
		break_stack.push_back(end.to_string());

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
		break_stack.pop_back();
	}

	else if (ast->type == "break") {
		emit("    j " + break_stack.back());
	}

	else if (ast->type == "return") {
		if (!ast->children.empty()) {
			gen_pass_1(ast->get_child(0));
			emit("    move $v0," + ast->get_child(0)->reg);
		}
		emit("    j " + current_func + "_epilogue");
	}

	else if (ast->type == "int") {
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    li " + reg + "," + ast->attr);
	}

	else if (ast->type == "u-") {
		auto reg = alloc_reg();
		ast->reg = reg;

		auto current = ast;
		while(current->type != "int") {
			current = current->get_child(0);
		}
		emit("    li " + reg + "," + current->attr);
	}

	else if (ast->type == "string") {
		auto reg = alloc_reg();
		ast->reg = reg;
		std::string str_global;
		auto normalized = (ast->attr == "\\t" || ast->attr == "\t") ? "\t" : ast->attr;
		if(string_to_global.count(normalized)) {
			str_global = string_to_global[normalized];
		} else {
			str_global = StrGlobal().to_string();
		}
		emit("    la " + reg + "," + str_global);
		global_to_string[str_global] = normalized;
		string_to_global[normalized] = str_global;
	}

	else if (ast->type == "id") {
		auto reg = alloc_reg();
		ast->reg = reg;
		if (ast->attr == "true" || ast->attr == "$true") {
			emit("    li " + reg + ",Ltrue");
		} else if (ast->attr == "false" && ast->sym->sig == "bool") {
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
		gen_pass_1(ast->get_child(0), true);
		gen_pass_1(ast->get_child(1), true);
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    seq " + reg + "," + ast->get_child(0)->reg + "," + ast->get_child(1)->reg);
		freereg(ast->get_child(1)->reg);
		freereg(ast->get_child(0)->reg);
	}

	else if (ast->type == "!=") {
		gen_pass_1(ast->get_child(0), true);
		gen_pass_1(ast->get_child(1), true);
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    sne " + reg + "," + ast->get_child(0)->reg + "," + ast->get_child(1)->reg);
		freereg(ast->get_child(1)->reg);
		freereg(ast->get_child(0)->reg);
	}

	else if (ast->type == ">=") {
		gen_pass_1(ast->get_child(0), true);
		gen_pass_1(ast->get_child(1), true);
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    sge " + reg + "," + ast->get_child(0)->reg + "," + ast->get_child(1)->reg);
		freereg(ast->get_child(1)->reg);
		freereg(ast->get_child(0)->reg);
	}

	else if (ast->type == ">") {
		gen_pass_1(ast->get_child(0), true);
		gen_pass_1(ast->get_child(1), true);
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    sgt " + reg + "," + ast->get_child(0)->reg + "," + ast->get_child(1)->reg);
		freereg(ast->get_child(1)->reg);
		freereg(ast->get_child(0)->reg);
	}

	else if (ast->type == "<=") {
		gen_pass_1(ast->get_child(0), true);
		gen_pass_1(ast->get_child(1), true);
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    sle " + reg + "," + ast->get_child(0)->reg + "," + ast->get_child(1)->reg);
		freereg(ast->get_child(1)->reg);
		freereg(ast->get_child(0)->reg);
	}

	else if (ast->type == "<") {
		gen_pass_1(ast->get_child(0), true);
		gen_pass_1(ast->get_child(1), true);
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    slt " + reg + "," + ast->get_child(0)->reg + "," + ast->get_child(1)->reg);
		freereg(ast->get_child(1)->reg);
		freereg(ast->get_child(0)->reg);
	}

	else if (ast->type == "*") {
		gen_pass_1(ast->get_child(0), true);
		gen_pass_1(ast->get_child(1), true);
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    mul " + reg + "," + ast->get_child(0)->reg + "," + ast->get_child(1)->reg);
		freereg(ast->get_child(1)->reg);
		freereg(ast->get_child(0)->reg);
	}

	else if (ast->type == "/") {
		gen_pass_1(ast->get_child(0), true);
		gen_pass_1(ast->get_child(1), true);
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
		gen_pass_1(ast->get_child(0), true);
		gen_pass_1(ast->get_child(1), true);
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
		gen_pass_1(ast->get_child(0), true);
		gen_pass_1(ast->get_child(1), true);
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    addu " + reg + "," + ast->get_child(0)->reg + "," + ast->get_child(1)->reg);
		freereg(ast->get_child(1)->reg);
		freereg(ast->get_child(0)->reg);
	}

	else if (ast->type == "-") {
		gen_pass_1(ast->get_child(0), true);
		gen_pass_1(ast->get_child(1), true);
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    subu " + reg + "," + ast->get_child(0)->reg + "," + ast->get_child(1)->reg);
		freereg(ast->get_child(1)->reg);
		freereg(ast->get_child(0)->reg);
	}

	else if (ast->type == "!") {
		gen_pass_1(ast->get_child(0), true);
		freereg(ast->get_child(0)->reg);
		auto reg = alloc_reg();
		ast->reg = reg;
		emit("    xori " + reg + "," + ast->get_child(0)->reg + ",1");
	}

	else if (ast->type == "=") {
		gen_pass_1(ast->get_child(1), true);
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
	std::map<char, char> escapes = {
		{'b' , '\b'},
		{'t' , '\t' },
		{'n' , '\n' },
		{'f' , '\f' },
		{'r' , '\r' },
		{'"' , '"' },
		{'\'', '\'' },
		{'\\' , '\\' },
	};

	if (global_to_string.empty()) {
		return;
	}

	// Sort strings by length
	std::vector<std::pair<std::string, std::string>> sorted;
	for (auto it = global_to_string.begin(); it != global_to_string.end(); it++) {
		sorted.push_back(*it);
	}
	std::sort(sorted.begin(), sorted.end(), [](std::pair<std::string, std::string>& a, std::pair<std::string, std::string>& b)
	{
		if (a.second.length() == b.second.length()) { // If length is the same
			int cmp = a.second.compare(b.second); // Compare alphabetically
			if (cmp == 0) { // If they are the same alphabetically
				// Compare case-insensitively
				return std::lexicographical_compare(a.second.begin(), a.second.end(), b.second.begin(), b.second.end(),
													[](char a, char b) { return std::tolower(a) < std::tolower(b); });
			} else {
				return cmp < 0;
			}
		} else {
			return a.second.length() < b.second.length();
		}
	});

	emit("    .data");
	auto escaping = false;
	for (auto &[label, value]: sorted) {
		emit(label + ":");
		for (char &c: value) {
			if(c == 92 && !escaping) {
				escaping = true;
			} else if(escaping && escapes.count(c)) {
				emit("    .byte " + std::to_string(escapes[c]));
				escaping = false;
			} else {
				emit("    .byte " + std::to_string(int(c)));
			}
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
	emit("    j halt");

	// Populate the globals
	gen_pass_0(root);

	// Majority of the code generation
	gen_pass_1(root);

	// Populate predefined functions
	get_char();
	prints();
	printi();
	halt();
	printb();
	printc();
	len();
	divmodchk();
	error();

	// String tomfoolery
	gen_pass_2();
}

void get_char(){
	if(redefined["getchar"])
		return;

	emit("    .data");
	emit("    char: .space 2");
	emit("    .text");
	emit("getchar:");
	emit("    addi $sp,$sp,-4");
	emit("    sw $s0,0($sp)");
	emit("    li $v0,8");
	emit("    la $a0,char");
	emit("    la $a1,2");
	emit("    syscall");
	emit("    lb $v0,char");
	emit("    li $s0,4");
	emit("    beq $v0,$s0,getchar_eof");
	emit("    li $s0,0");
	emit("    beq $v0,$s0,getchar_eof");
	emit("getchar_epilogue:");
	emit("    lw $s0,0($sp)");
	emit("    addi $sp,$sp,4");
	emit("    jr $ra ");
	emit("getchar_eof:");
	emit("    li $v0,-1");
	emit("    j getchar_epilogue");
}

void prints(){
	if(redefined["prints"])
		return;

	emit("prints:");
	emit("    li $v0,4");
	emit("    syscall");
	emit("    jr $ra ");
}

void printi(){
	if(redefined["printi"])
		return;

	emit("printi:");
	emit("    li $v0,1");
	emit("    syscall");
	emit("    jr $ra ");

}

void halt(){
	if(redefined["halt"])
		return;

	emit("halt:");
	emit("    li $v0,10");
	emit("    syscall");
	emit("    jr $ra ");

}

void printb(){
	if(redefined["printb"])
		return;

	auto t = StrGlobal();
	auto f = StrGlobal();

	string_to_global["true"] = t.to_string();
	global_to_string[t.to_string()] = "true";
	string_to_global["false"] = f.to_string();
	global_to_string[f.to_string()] = "false";

	emit("printb:");
	emit("	  li $t0,1");
	emit("	  beq $a0,$zero,printb_false");
	emit("	  la $a0," + t.to_string());
	emit("	  j printb_epilogue");
	emit("printb_false:");
	emit("	  la $a0," + f.to_string());
	emit("printb_epilogue:");
	emit("	  li $v0,4");
	emit("	  syscall");
	emit("	  jr $ra");
}

void printc(){
	if(redefined["printc"])
		return;

	emit("printc:");
	emit("    li $v0,11");
	emit("    syscall");
	emit("    jr $ra ");
}

void len(){
	if(redefined["len"])
		return;

	emit("len:");
	emit("    subu $sp,$sp,8");
	emit("    sw $ra,0($sp)");
	emit("    sw $a0,4($sp)");
	emit("    li $t0,0");
	emit("len_loop:");
	emit("    lb $t1,0($a0)");
	emit("    beqz $t1,len_epilogue");
	emit("    addi $a0,$a0,1");
	emit("    addi $t0,$t0,1");
	emit("    j len_loop");
	emit("len_epilogue:");
	emit("    move $v0,$t0");
	emit("    lw $ra,0($sp)");
	emit("    addu $sp,$sp,8");
	emit("    jr $ra ");
}

void divmodchk(){
	if(redefined["divmodchk"])
		return;

	auto err = StrGlobal();
	global_to_string[err.to_string()] = "error: division by zero\n";

	emit("divmodchk:");
	emit("    subu $sp,$sp,12");
	emit("    sw $ra,0($sp)");
	emit("    sw $a0,4($sp)");
	emit("    sw $a1,8($sp)");
	emit("    bne $a1,$zero,divmodchk_min");
	emit("    la $a0," + err.to_string());
	emit("    li $v0,4");
	emit("    syscall");
	emit("    j halt");
	emit("divmodchk_min:");
	emit("    bne $a0,-2147483648,divmodchk_epilogue");
	emit("    li $a1,1");
	emit("divmodchk_epilogue:");
	emit("    move $v0,$a1");
	emit("    lw $ra,0($sp)");
	emit("    addu $sp,$sp,12");
	emit("    jr $ra ");
}

void error(){
	if(redefined["error"])
		return;

	emit("error:");
	emit("    li $v0,4");
	emit("    syscall");
	emit("    j halt");
}