#pragma once

#include <string>
#include <vector>
#include <functional>
#include "record.h"

class AST {
public:
    std::string attr;
    std::string type;
	std::string sig;
	std::string reg;
	Record* sym;
    int line;
    int column;
    std::vector<AST*> children;

    AST(std::string type);
    AST(std::string type, std::string attr);
    AST(std::string type, std::string attr, int line, int column);
    AST(std::string type, int line, int column);
    AST* add_child(AST* child);
    AST* get_child(int index);
    void print();
    void pre(std::function<void(AST *ast)> callback);
    void post(std::function<void(AST *ast)> callback);
    void pre_post(std::function<void(AST *ast)> pre_callback, std::function<void(AST *ast)> post_callback);

private:
    void print(int indent);
};
