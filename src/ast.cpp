#include "ast.h"
#include<iostream>
#include <sstream>

AST::AST(std::string type, std::string attr, int line, int column) : type(type), attr(attr), line(line),
                                                                     column(column), sym(nullptr) {}

AST::AST(std::string type) : AST(type, "", -1, -1) {}

AST::AST(std::string type, std::string attr) : AST(type, attr, -1, -1) {}

AST::AST(std::string type, int line, int column) : AST(type, "", line, column) {}

AST *AST::add_child(AST *child) {
    children.push_back(child);
    return this;
}

AST *AST::get_child(int index) {
    return children.at(index);
}

void AST::print() {
    print(0);
}

void AST::print(int indent) {
    // Indentation
    std::stringstream ss;
    ss << std::string(indent, '\t');
    ss << type;

    // AST has an attribute
    if (attr.length() > 0)
        ss << " [" << attr << "]";

    // AST has a sig(nature type)
    if (sig.length() > 0)
        ss << " sig=" << sig;

    // AST has a sym(bol table reference)
    if (sym != NULL)
        ss << " sym=" << sym;

    // AST has location data
    if (line >= 0)
        ss << " @ (" << line << ", " << column << ")";

    // Print string and print children
    std::cout << ss.str() << std::endl;
    for (auto child: children)
        child->print(indent + 1);
}

void AST::pre(std::function<void(AST *ast)> callback) {
    pre_post(callback, [](auto) {});
}

void AST::post(std::function<void(AST *ast)> callback) {
    pre_post([](auto) {}, callback);
}

void AST::pre_post(std::function<void(AST *ast)> pre_callback, std::function<void(AST *ast)> post_callback) {
    pre_callback(this);
    for (auto child: children)
        child->pre_post(pre_callback, post_callback);
    post_callback(this);
}