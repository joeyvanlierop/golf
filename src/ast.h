#pragma once

#include <string>
#include <vector>

class AST {
public:
    AST(std::string type);
    AST(std::string type, std::string attr);
    AST(std::string type, std::string attr, int line, int column);
    void add_child(AST* child);
    void print();

private:
    std::vector<AST*> children;
    std::string type;
    std::string attr;
    int line;
    int column;

    void print(int indent);
};
