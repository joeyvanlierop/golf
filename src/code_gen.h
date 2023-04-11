#pragma once

#include <string>
#include <iostream>

#include "ast.h"

void get_char();
void prints();
void printi();
void halt();
void printb();
void printc();

void gen_pass1(AST * node, int direction);
void gen_pass2(AST * node, int direction);

std::string get_reg(AST * node);
std::string get_identifier_addr(std::string id);

void customTraverse(AST * root, void (*callback)(AST *, int));
void traverse(AST * root);