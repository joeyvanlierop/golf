#include <sstream>
#include "parser.h"
#include "logger.h"

Parser::Parser(FileReader *filereader, std::vector<Token> tokens) : filereader(filereader), tokens(tokens) {}

bool Parser::is_at_end() {
    return peek().type == Eof;
}

Token Parser::advance() {
    if(!is_at_end())
        current++;
    return peek();
}

Token Parser::peek() {
    return tokens.at(current);
}

Token Parser::consume(TokenType type) {
    if(check(type))
        return advance();

    std::ostringstream buffer;
    buffer << "expected " << type << ", got " << peek().type << std::endl;
    Logger::error(filereader, peek().line, peek().column, 1, buffer.str());
}

bool Parser::check(TokenType type) {
    if(is_at_end())
        return false;
    return peek().type == type;
}

/**
 * Program ::= { Declaration } EOF
 */
AST *Parser::parse() {
    auto ast = new AST("program");
    while(!is_at_end()) {
        auto child = decl();
        ast->add_child(child);
    }
    return ast;
}

/**
 * Declaration ::= VarDecl | FuncDecl
 */
AST *Parser::decl() {
    switch (peek().type) {
        case Var:
            return var_decl();
        case Func:
            return func_decl();
        default:
            Logger::error(filereader, peek().line, peek().column, 1, "unexpected token");
    }
}

/**
 * VarDecl ::= "var" identifier [ identifier ]
 */
AST *Parser::var_decl() {
    auto ast = new AST("var");

    // Token representing the name of the variable
    auto id = consume(Identifier);
    ast->add_child(new AST("newid", id.lexeme, id.line, id.column));

    // Optional token representing the type of the variable
    auto has_type = check(Identifier);
    if (has_type){
        auto type = consume(Identifier);
        ast->add_child(new AST("typeid", type.lexeme, type.line, type.column));
    }

    return ast;
}

/**
 * FuncDecl ::= "func" identifier Signature Block
 */
AST *Parser::func_decl() {
    auto ast = new AST("func");
    return ast;
}

/**
 * Signature ::= "(" { identifier identifier "," } ")" [ identifier ]
 */
AST *Parser::func_sig() {
    auto ast = new AST("sig");
    return ast;
}

/**
 * Block ::= "{" { Statement } "}"
 */
AST *Parser::block() {
    auto ast = new AST("block");
    return ast;
}

/**
 * Statement ::=
 *  | VarDecl
 *  | IfStmt
 *  | ForStmt
 *  | BreakStmt
 *  | ReturnStmt
 *  | Block
 *  | Expression
 *  | Assignment
 */
AST *Parser::stmt() {
    switch(peek().type) {
        case Var:
            return var_decl();
        case If:
            return if_stmt();
        case For:
            return for_stmt();
        case Break:
            return break_stmt();
        case Return:
            return return_stmt();
        case LeftBracket:
            return block();
    }
}

/**
 * IfStmt ::= "if" Expression block [ "else" IfStmt | block ]
 */
AST *Parser::if_stmt() {
    auto ast = new AST("if");
    return ast;
}

/**
 * ForStmt ::= "for" Expression block
 */
AST *Parser::for_stmt() {
    auto ast = new AST("for");
    return ast;
}

/**
 * BreakStmt ::= "break"
 */
AST *Parser::break_stmt() {
    auto ast = new AST("break");
    return ast;
}

/**
 * ReturnStmt ::= "return" [ Expression ]
 */
AST *Parser::return_stmt() {
    auto ast = new AST("return");
    return ast;
}

/**
 * Expression ::= UnaryExpr | Expression binary_op Expression
 */
AST *Parser::expr() {
    auto ast = new AST("expr");
    return ast;
}

/**
 * UnaryExpr ::= PrimaryExpr | unary_op UnaryExpr
 */
AST *Parser::unary_expr() {
    auto ast = new AST("expr");
    return ast;
}

/**
 * PrimaryExpr ::= Operand | PrimaryExpr Arguments
 */
AST *Parser::primary_expr() {
    auto ast = new AST("primary");
    return ast;
}

/**
 * Operand ::= int_lit | string_lit | identifier | "(" Expression ")"
 */
AST *Parser::operand() {
    auto ast = new AST("operand");
    return ast;
}

/**
 * Arguments ::= "(" Expression { "," Expression } [ "," ] ")"
 */
AST *Parser::arguments() {
    auto ast = new AST("operand");
    return ast;
}

/**
 * Assignment ::= Expression "=" Expression
 */
AST *Parser::assignment() {
    auto ast = new AST("operand");
    return ast;
}