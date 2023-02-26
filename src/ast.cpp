#include "ast.h"
#include<iostream>
#include <sstream>

AST::AST(std::string type, std::string attr, int line, int column) : type(type), attr(attr), line(line),
                                                                     column(column) {}

AST::AST(std::string type) : AST(type, "", 0, 0) {}

void AST::add_child(AST *child) {
    this->children.push_back(child);
}

void AST::print() {
    print(0);
}

void AST::print(int indent) {
    std::stringstream ss;
    ss << std::string(indent, '\t');
    ss << type;
    if(attr.length() > 0)
        ss << " [" << attr << "]";
    ss << " @ (" << line << ", "
       << column << ")";
    std::cout << ss.str() << std::endl;
    for (auto child: children)
        child->print(indent + 1);
}
