#include <stdio.h>
#include <iostream>
#include <set>
#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>

#include "code_gen.h"

int global_counter = 0;
int label_counter = 1;

using namespace std;

unordered_map<char, char> escape_map;

vector<string> all_regs {
		"$t0",
		"$t1",
		"$t2",
		"$t3",
		"$t4",
		"$t5",
		"$t6",
		"$t7",
		"$t8",
		"$t9",
		"$s0",
		"$s1",
		"$s2",
		"$s3",
		"$s4",
		"$s5",
		"$s6",
		"$s7"
};
unordered_map<string, vector<string>> registers;

unordered_map<string, string> var_addresses;
unordered_map<string, string> str_addresses;
unordered_map<string, string> func_labels;
unordered_map<string, int> func_stack_size;

string current_func = "";
string current_func_type = "";

int stack_decrement = 0;

vector<string> while_start;
vector<string> while_end;

vector<string> if_end;

vector<string> ifelse_else;
vector<string> ifelse_end;

int total_reg;

void register_error(){

	cerr << "Expression too complex." << endl;
	exit(1);

}

void populate_escape_map() {
	escape_map['b'] = '\b';
	escape_map['t'] = '\t';
	escape_map['n'] = '\n';
	escape_map['f'] = '\f';
	escape_map['r'] = '\r';
	escape_map['"'] = '"';
	escape_map['\''] = '\'';
	escape_map['\\'] = '\\';
}

void populate_registers(string func){
	for (auto r : all_regs) {
		registers[func].push_back(r);
	}
}

string allocreg(){
	if (registers.find(current_func) == registers.end()) {
		populate_registers(current_func);
	}

	if (!registers[current_func].empty()){

		string available_reg = registers[current_func].back();
		registers[current_func].pop_back();

		// cout << "    # Allocated reg: " << available_reg << endl;

		return available_reg;

	}

	else {
		cout << "Too complex - not enough free registers in " << current_func << " scope" << endl;
		exit(1);
	}
}

void freereg(string reg){
	if (reg != "" && reg.substr(0, 2) != "$v" && reg.substr(0, 2) != "$a") {
		// cout << "    # Deallocated reg: " << reg << endl;
		registers[current_func].push_back(reg);
	}
}

void print_predefs(){
	get_char();
	prints();
	printi();
	halt();
	printb();
	printc();
}

void get_char(){
	func_labels["getchar"] = "Lgetchar";
	cout << ".data" << endl;
	cout << "char: .space 2" << endl;
	cout << ".text" << endl;

	cout << "Lgetchar: " << endl;
	cout << "  addi $sp,$sp,-4" << endl;
	cout << "  sw $s0,0($sp)" << endl;
	cout << "  li $v0,8" << endl;
	cout << "  la $a0,char" << endl;
	cout << "  la $a1,2" << endl;
	cout << "  syscall" << endl;
	cout << "  lb $v0,char" << endl;
	cout << "  li $s0,4" << endl;
	cout << "  beq $v0,$s0,Lgetchar_eof" << endl;
	cout << "  li $s0,0" << endl;
	cout << "  beq $v0,$s0,Lgetchar_eof" << endl;
	cout << "Lgetchar_return:" << endl;
	cout << "  lw $s0 0($sp)" << endl;
	cout << "  addi $sp,$sp,4" << endl;
	cout << "  jr $ra " << endl;
	cout << "Lgetchar_eof:" << endl;
	cout << "  li $v0,-1" << endl;
	cout << "  j Lgetchar_return" << endl;
}

void prints(){
	func_labels["prints"] = "Lprints";
	cout << "Lprints: " << endl;
	cout << "  li $v0,4" << endl;
	cout << "  syscall" << endl;
	cout << "  jr $ra " << endl;
}

void printi(){
	func_labels["printi"] = "Lprinti";
	cout << "Lprinti: " << endl;
	cout << "  li $v0,1" << endl;
	cout << "  syscall" << endl;
	cout << "  jr $ra " << endl;

}

void halt(){
	func_labels["halt"] = "Lhalt";
	cout << ".data" << endl;
	cout << "return_err: .asciiz \"Runtime error: Function did not return\\n\"" << endl;
	cout << "divzero_err: .asciiz \"Runtime error: Division by zero\\n\"" << endl;
	cout << ".text" << endl;

	cout << "Lhalt: " << endl;
	cout << "  li $v0,10" << endl;
	cout << "  syscall" << endl;
	cout << "  jr $ra " << endl;

}

void printb(){
	func_labels["printb"] = "Lprintb";
	cout << "Lprintb: " << endl;
	cout << "  li $v0,1" << endl;
	cout << "  syscall" << endl;
	cout << "  jr $ra " << endl;

}

void printc(){
	func_labels["printc"] = "Lprintc";
	cout << "Lprintc: " << endl;
	cout << "  li $v0,11" << endl;
	cout << "  syscall" << endl;
	cout << "  jr $ra " << endl;

}

void gen_pass1(AST * node, int direction){
	if (!direction) {
		// Direction going down.
		if (node->type == "globalVarDecl"){
			if (node->get_child(0)->type == "int"){

				string label = "G" + to_string(global_counter);

				cout << ".data" << endl;
				cout << label << ":" << endl;
				cout << "   .word 0" << endl;
				cout << ".text" << endl;

				global_counter++;

				var_addresses[node->get_child(1)->attr] = label;
			} else if (node->get_child(0)->type == "boolean") {
				string label = "G" + to_string(global_counter);

				cout << ".data" << endl;
				cout << label << ":" << endl;
				cout << "   .word 0" << endl;
				cout << ".text" << endl;

				global_counter++;

				var_addresses[node->get_child(1)->attr] = label;
			}
		} else if (node->type == "string"){
			string text = node->attr;

			// Strip quotes;
			string no_quote_text = text.substr(1, text.length()-2);

			string label = "G" + to_string(global_counter);
			cout << ".data" << endl;
			cout << label << ":" << endl;
			cout << "  # " << text << endl;
			cout << "  .byte ";

			string to_print = "";

			string::iterator it = no_quote_text.begin();

			while(it != no_quote_text.end()) {
				char c = *it;
				if (c == '\\') {
					to_print += to_string(int(escape_map[*(it+1)]));
					it++;
				} else {
					to_print += to_string(int(*it));
				}
				to_print += " , ";
				it++;
			}

			to_print += "0";
			cout << to_print << endl;
			cout << "  .align 2" << endl;
			cout << ".text" << endl;

			global_counter++;

			str_addresses[text] = label;
		} else if (node->type == "funcDecl") {
			current_func = node->get_child(0)->attr;

			func_labels[current_func] = "L" + to_string(label_counter);
			label_counter++;
		} else if (node->type == "mainDecl") {
			current_func = "main";
		} else if (node->type == "varDecl" || node->type == "formal") {
			stack_decrement += 4;
			string id = node->get_child(1)->attr;
			var_addresses[current_func + "-" + id] = to_string(stack_decrement) + "($sp)";
			// cout << current_func + "-" + id << " : " << var_addresses[current_func + "-" + id] << endl;
		}
	} else {
		// Direction going up.
		if (node->type == "mainDecl" || node->type == "funcDecl") {
			func_stack_size[current_func] = stack_decrement + 4;
			current_func = "";
			stack_decrement = 0;
		}
	}
}

void print_pass1() {
	cout << "==== var addresses ====" << endl;
	for (auto const &p : var_addresses) {
		cout << p.first << " : " << p.second << endl;
	}

	cout << "==== str addresses ====" << endl;
	for (auto const &p : str_addresses) {
		cout << p.first << " : " << p.second << endl;
	}

	cout << "==== func stack size ====" << endl;
	for (auto const &p : func_stack_size) {
		cout << p.first << " : " << p.second << endl;
	}
}

void gen_pass2(AST * node, int direction){
	// cout << node->type << " direction: " << direction << endl;

	if (!direction){
		// Direction going down.
		if (node->type == "mainDecl") {
			current_func = "main";
			cout << "L0: # main" << endl;
			cout << "  addi $sp,$sp,-" << func_stack_size[current_func] << endl;
			cout << "  sw $ra,0($sp)" << endl;
		} else if (node->type == "func") {
			current_func = node->get_child(0)->attr;

			if (node->get_child(1)->type == "formals") {
				current_func_type = node->get_child(2)->type;
			} else {
				current_func_type = node->get_child(1)->type;
			}

			cout << func_labels[current_func] << ": # " << current_func << endl;

			// Allocate space on the stack for regs
			cout << "  addi $sp,$sp,-" << func_stack_size[current_func] + (4 * all_regs.size()) << endl;
			cout << "  sw $ra,0($sp)" << endl;

			if (node->get_child(1)->type == "formals") {
				int formal_count = 0;
				for (auto const formal : node->get_child(1)->children) {
					string id = formal->get_child(1)->attr;
					string addr = get_identifier_addr(id);

					cout << "  sw $a" << formal_count << "," << addr << endl;

					formal_count++;

					if (formal_count > 4) {
						cout << "Too complex - function " << current_func << " has more than 4 arguments" << endl;
						exit(1);
					}
				}
			}

			int i = 0;
			for (auto r : all_regs) {
				cout << "  sw " << r << "," << func_stack_size[current_func] + (i * 4) << "($sp)" << endl;
				i++;
			}

		} else if (node->type == "int"){
			string reg = allocreg();
			cout << "  li " << reg << "," << node->attr << endl;
			node->reg = reg;
		} else if (node->type == "true") {
			string reg = allocreg();
			cout << "  li " << reg << ",1" << endl;
			node->reg = reg;
		} else if (node->type == "false") {
			string reg = allocreg();
			cout << "  li " << reg << ",0" << endl;
			node->reg = reg;
		} else if (node->type == "while") {
			string start_label = "L" + to_string(label_counter);
			while_start.push_back(start_label);
			label_counter++;

			string end_label = "L" + to_string(label_counter);
			while_end.push_back(end_label);
			label_counter++;

			cout << start_label << ": # " << node->type << " start" << endl;

		} else if (node->type == "if") {
			// start label just used to debug
			string start_label = "L" + to_string(label_counter);
			label_counter++;
			cout << start_label << ": # " << node->type << " start" << endl;

			string end_label = "L" + to_string(label_counter);
			if_end.push_back(end_label);
			label_counter++;

		} else if (node->type == "ifelse") {
			// start label just used to debug
			string start_label = "L" + to_string(label_counter);
			label_counter++;
			cout << start_label << ": # " << node->type << " start" << endl;

			string else_label = "L" + to_string(label_counter);
			ifelse_else.push_back(else_label);
			label_counter++;

			string end_label = "L" + to_string(label_counter);
			ifelse_end.push_back(end_label);
			label_counter++;

		} else if (node->type == "break") {
			cout << "j " << while_end.back() << endl;
		} else if (node->type == "block") {
			for (auto a : node->children) {
				a->parent_id = node->type;
			}
		}
	} else {
		// Direction going up.
		if (node->type == "mainDecl") {
			cout << "  lw $ra,0($sp)" << " # b" << endl;
			cout << "  addi $sp,$sp," << func_stack_size[current_func] << endl;
			cout << "  jr $ra" << endl;
			current_func = "";
		} else if (node->type == "func") {
			// Add return runtime error.
			if (current_func_type != "void") {
				cout << "  la $a0,return_err" << endl;
				cout << "  jal Lprints" << endl;
				cout << "  jal Lhalt" << endl;
			}

			// Cleanup stack
			cout << func_labels[current_func] << "CLEANUP" << ": # " << current_func << " return" << endl;

			int i = 0;
			for (auto r : all_regs) {
				cout << "  lw " << r << "," << func_stack_size[current_func] + (i * 4) << "($sp)" << endl;
				i++;
			}

			cout << "  lw $ra,0($sp)" << " # c" << endl;
			cout << "  addi $sp,$sp," << func_stack_size[current_func] + (all_regs.size() * 4) << endl;
			cout << "  jr $ra" << endl;
			current_func = "";
			current_func_type = "";
		} else if (node->type == "-"){
			cout << "L" << label_counter << ": # " << node->type << endl;
			label_counter++;

			string op_reg = allocreg();

			if (node->children.size() == 1) {
				string reg = get_reg(node->get_child(0));
				cout << "  negu " << op_reg << "," << reg << endl;
				freereg(reg);
			} else {
				string l_reg = get_reg(node->get_child(0));
				string r_reg = get_reg(node->get_child(1));
				cout << "  subu " << op_reg << "," << l_reg << "," << r_reg << endl;

				freereg(l_reg);
				freereg(r_reg);
			}

			node->reg = op_reg;

		} else if (node->type == "!"){
			cout << "L" << label_counter << ": # " << node->type << endl;
			label_counter++;

			string op_reg = allocreg();
			string reg = get_reg(node->get_child(0));

			cout << "  xori " << op_reg << "," << reg << "," << "1" << endl;

			node->reg = op_reg;

			freereg(reg);

		} else if (node->type == "+"){
			cout << "L" << label_counter << ": # " << node->type << endl;
			label_counter++;

			string op_reg = allocreg();

			string l_reg = get_reg(node->get_child(0));
			string r_reg = get_reg(node->get_child(1));

			cout << "  addu " << op_reg << "," << l_reg << "," << r_reg << endl;
			node->reg = op_reg;

			freereg(l_reg);
			freereg(r_reg);

		} else if (node->type == "*"){
			cout << "L" << label_counter << ": # " << node->type << endl;
			label_counter++;

			string op_reg = allocreg();

			string l_reg = get_reg(node->get_child(0));
			string r_reg = get_reg(node->get_child(1));

			cout << "  mul " << op_reg << "," << l_reg << "," << r_reg << endl;

			node->reg = op_reg;

			freereg(l_reg);
			freereg(r_reg);

		} else if (node->type == "/"){
			cout << "L" << label_counter << ": # " << node->type << endl;
			label_counter++;

			string op_reg = allocreg();

			string l_reg = get_reg(node->get_child(0));
			string r_reg = get_reg(node->get_child(1));

			string div_zero_skip = "L" + to_string(label_counter);
			label_counter++;

			cout << "  bne " << r_reg << ",$zero," << div_zero_skip << endl;
			cout << "  la $a0,divzero_err" << endl;
			cout << "  jal Lprints" << endl;
			cout << "  jal Lhalt" << endl;

			cout << div_zero_skip << ": # " << node->type << " divzero skip" << endl;
			cout << "  div " << l_reg << "," << r_reg << endl;
			cout << "  mflo " << op_reg << endl;

			node->reg = op_reg;

			freereg(l_reg);
			freereg(r_reg);

		} else if (node->type == "%"){
			cout << "L" << label_counter << ": # " << node->type << endl;
			label_counter++;

			string op_reg = allocreg();

			string l_reg = get_reg(node->get_child(0));
			string r_reg = get_reg(node->get_child(1));

			cout << "  div " << l_reg << "," << r_reg << endl;
			cout << "  mfhi " << op_reg << endl;

			node->reg = op_reg;

			freereg(l_reg);
			freereg(r_reg);
		} else if (node->type == "="){
			cout << "L" << label_counter << ": # " << node->type << endl;
			label_counter++;

			string l_identifier = node->get_child(0)->attr;
			string l_addr = get_identifier_addr(l_identifier);

			AST * r_node = node->get_child(1);

			string r_reg = get_reg(r_node);

			cout << "  sw " << r_reg << "," << l_addr << endl;

			// We don't store a reg on the = node. Instead, the lhs identifier must be re-read from memory.
			// This is done by the get_reg helper function.

			freereg(r_reg);
		} else if (node->type == "funcCall") {
			string func_name = node->get_child(0)->attr;
			if (node->children.size() > 1) {
				int current_arg = 0;
				for (auto const n : node->get_child(1)->children) {
					if (current_arg >= 4) {
						cerr << "too many arguments in the function" << endl;
						exit(1);
					}
					if (n->type == "string") {
						string str_label = str_addresses[n->attr];
						cout << "  la $a" << current_arg << "," << str_label << endl;
					} else {
						string reg = get_reg(n);
						cout << "  move $a" << current_arg << "," << reg << endl;
						freereg(reg); // DOUBLE CHECK THIS!!!
					}
					current_arg++;
				}
			}
			cout << "  jal " << func_labels[func_name] << endl;

			if (node->parent_id != "block") {
				string new_reg = allocreg();
				cout << "  move " << new_reg << "," << "$v0" << endl;
				node->reg = new_reg;
			}

		} else if (node->type == "return") {
			if (!node->children.empty()) {
				string reg = get_reg(node->get_child(0));
				cout << "  move $v0," << reg << endl;
			}
			cout << "  j " << func_labels[current_func] << "CLEANUP" << endl;
		} else if (node->type == "<" || node->type == "<=" || node->type == ">" || node->type == ">=" || node->type == "==" || node->type == "!=") {
			cout << "L" << label_counter << ": # " << node->type << endl;
			label_counter++;

			string op_reg = allocreg();

			string l_reg = get_reg(node->get_child(0));
			string r_reg = get_reg(node->get_child(1));

			string skip_label_1 = "L" + to_string(label_counter);
			label_counter++;
			string skip_label_2 = "L" + to_string(label_counter);
			label_counter++;

			string opt = "";
			if (node->type == "<") {
				opt = "blt";
			} else if (node->type == "<=") {
				opt = "ble";
			} else if (node->type == ">") {
				opt = "bgt";
			} else if (node->type == ">=") {
				opt = "bge";
			} else if (node->type == "==") {
				opt = "beq";
			} else if (node->type == "!=") {
				opt = "bne";
			}

			cout << "  " << opt << " " << l_reg << "," << r_reg << "," << skip_label_1 << endl;

			cout << "  li " << op_reg << ",0" << endl;
			cout << "  j " << skip_label_2 << endl;
			cout << skip_label_1 << ":" << endl;
			cout << "  li " << op_reg << ",1" << endl;
			cout << skip_label_2 << ":" << endl;

			freereg(l_reg);
			freereg(r_reg);

			node->reg = op_reg;
		} else if (node->type == "while") {
			cout << "j " << while_start.back() << endl;
			cout << while_end.back() << ": # " << node->type << " end" << endl;

			while_start.pop_back();
			while_end.pop_back();

			// Free the expression node's register;
			freereg(node->get_child(0)->reg);
		} else if (node->type == "if") {
			cout << if_end.back() << ": #" << node->type << " end" << endl;

			if_end.pop_back();

			// Free the expression node's register;
			freereg(node->get_child(0)->reg);
		} else if (node->type == "ifelse") {
			cout << ifelse_end.back() << ": #" << node->type << " end" << endl;

			ifelse_else.pop_back();
			ifelse_end.pop_back();

			// Free the expression node's register;
			freereg(node->get_child(0)->reg);
		}
	}
}

string get_identifier_addr(string id) {
	string addr = var_addresses[id];
	if (addr == "") {
		addr = var_addresses[current_func + "-" + id];
	}

	return addr;
}

string get_reg(AST * node) {
	if (node->type == "stmtExpr") {
		// Here we need to read the previously assigned value from memory into a new reg.
		string new_reg = allocreg();
		string id = node->get_child(0)->get_child(0)->attr;
		string addr = get_identifier_addr(id);
		cout << "  lw " << new_reg << "," << addr << endl;

		return new_reg;

	} else if (node->type == "identifier") {
		string new_reg = allocreg();
		string id = node->attr;
		string addr = get_identifier_addr(id);
		cout << "  lw " << new_reg << "," << addr << endl;

		return new_reg;
	} else {
		// Node has reg as attribute.
		return node->reg;
	}
}

void customTraverse(AST * root, void (*callback)(AST *, int)){

	callback(root, 0);

	if (root->type == "if") {
		customTraverse(root->get_child(0), callback);
		string expr_reg = get_reg(root->get_child(0));
		cout << "  beq " << expr_reg << ",$zero," << if_end.back() << endl;
		customTraverse(root->get_child(1), callback);

	} else if (root->type == "ifelse") {
		customTraverse(root->get_child(0), callback);
		string expr_reg = get_reg(root->get_child(0));
		cout << "  beq " << expr_reg << ",$zero," << ifelse_else.back() << endl;
		customTraverse(root->get_child(1), callback);
		cout << "  j " << ifelse_end.back() << endl;
		cout << ifelse_else.back() << ": # " << root->type << " else" << endl;
		customTraverse(root->get_child(2), callback);

	} else if (root->type == "while") {
		customTraverse(root->get_child(0), callback);
		string expr_reg = get_reg(root->get_child(0));
		cout << "  beq " << expr_reg << ",$zero," << while_end.back() << endl;
		customTraverse(root->get_child(1), callback);

	} else if (root->type == "&&") {
		customTraverse(root->get_child(0), callback);

		string skip_label = "L" + to_string(label_counter);
		label_counter++;

		string op_reg = allocreg();

		string l_reg = get_reg(root->get_child(0));
		cout << "  li " << op_reg << ",0" << endl;
		cout << "  beq " << l_reg << ",$zero," << skip_label << endl;

		customTraverse(root->get_child(1), callback);
		string r_reg = get_reg(root->get_child(1));
		cout << "  beq " << r_reg << ",$zero," << skip_label << endl;
		cout << "  li " << op_reg << ",1" << endl;

		cout << skip_label << ": # " << root->type << " skip" << endl;

		root->reg = op_reg;

	} else if (root->type == "||") {
		customTraverse(root->get_child(0), callback);

		string skip_label = "L" + to_string(label_counter);
		label_counter++;

		string op_reg = allocreg();

		string l_reg = get_reg(root->get_child(0));
		cout << "  li " << op_reg << ",1" << endl;
		cout << "  bne " << l_reg << ",$zero," << skip_label << endl;

		customTraverse(root->get_child(1), callback);
		string r_reg = get_reg(root->get_child(1));
		cout << "  bne " << r_reg << ",$zero," << skip_label << endl;
		cout << "  li " << op_reg << ",0" << endl;

		cout << skip_label << ": # " << root->type << " skip" << endl;

		root->reg = op_reg;

	} else {
		for (auto a : root->children){
			customTraverse(a, callback);
		}
	}

	callback(root, 1);

}

void traverse(AST * root){
	// populate_registers();

	cout << ".text" << endl;
	cout << ".globl main" << endl;
	cout << "main:" << endl;
	cout << "  jal L0" << endl;
	cout << "  li $v0,10" << endl;
	cout << "  syscall" << endl;

	populate_escape_map();
//	root->pre(gen_pass1);
	root->pre([](auto ast) {
		gen_pass1(ast, false);
	});
	// print_pass1();

	print_predefs();

	customTraverse(root, gen_pass2);
}