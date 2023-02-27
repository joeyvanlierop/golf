#include <sstream>
#include "parser.h"
#include "logger.h"

Parser::Parser(FileReader *filereader, std::vector<Token> tokens) : filereader(filereader), tokens(tokens) {}

bool Parser::is_at_end() {
    return peek().type == Eof;
}

Token Parser::advance() {
    if (!is_at_end())
        current++;
    return peek();
}

Token Parser::peek() {
    return tokens.at(current);
}

Token Parser::consume(TokenType type) {
    auto curr = peek();
    if (check(type)) {
        advance();
        return curr;
    }

    std::stringstream ss;
    ss << "expected " << type << ", got " << peek().type;

    Logger::error(filereader, curr.line, curr.column + 1, curr.lexeme.length(), ss.str());
}

bool Parser::check(TokenType type) {
    if (is_at_end())
        return false;
    return peek().type == type;
}

bool Parser::match(TokenType expected) {
    if (peek().type != expected)
        return false;
    advance();
    return true;
}

/**
 * Program ::= { Declaration } EOF
 */
AST *Parser::parse() {
    auto ast = new AST("program");
    while (!is_at_end()) {
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
        case Var:return var_decl();
        case Func:return func_decl();
        default:
            Logger::error(filereader, peek().line, peek().column + 1, 1, "unexpected token [" + peek().lexeme + "]");
    }
}

/**
 * VarDecl ::= "var" identifier [ identifier ] ";"
 */
AST *Parser::var_decl() {
    auto ast = new AST("var");
    consume(Var);

    // Variable name
    auto id = consume(Identifier);
    ast->add_child(new AST("newid", id.lexeme, id.line, id.column));

    // Optional variable type
    auto has_type = check(Identifier);
    if (has_type) {
        auto type = consume(Identifier);
        ast->add_child(new AST("typeid", type.lexeme, type.line, type.column));
    }

    consume(Semicolon);
    return ast;
}

/**
 * FuncDecl ::= "func" identifier Signature Block
 */
AST *Parser::func_decl() {
    auto ast = new AST("func");
    consume(Func);

    // Function name
    auto id = consume(Identifier);
    ast->add_child(new AST("newid", id.lexeme, id.line, id.column));

    // Function signature
    auto signature = func_sig();
    ast->add_child(signature);

    // Function body
    auto body = block();
    ast->add_child(body);

    consume(Semicolon);
    return ast;
}

/**
 * Signature ::= "(" { identifier identifier "," } ")" [ identifier ]
 */
AST *Parser::func_sig() {
    auto ast = new AST("sig");

    // Opening paren
    consume(LeftParen);

    // Formals
    auto formals = new AST("formals");
    ast->add_child(formals);
    do {
        auto formal = new AST("formal");

        auto id = consume(Identifier);
        formal->add_child(new AST("newid", id.lexeme, id.line, id.column));
        auto type = consume(Identifier);
        formal->add_child(new AST("typeid", type.lexeme, type.line, type.column));

        formals->add_child(formal);
    } while (match(Comma));

    // Closing paren
    consume(RightParen);

    // Optional return type
    auto has_type = check(Identifier);
    if (has_type) {
        auto type = consume(Identifier);
        ast->add_child(new AST("typeid", type.lexeme, type.line, type.column));
    } else {
        ast->add_child(new AST("typeid", "$void"));
    }

    return ast;
}

/**
 * Block ::= "{" { Statement } "}"
 */
AST *Parser::block() {
    auto ast = new AST("block");
    consume(LeftBracket);

    // Block body
    while (!check(RightBracket)) {
        auto child = stmt();
        ast->add_child(child);
    }

    consume(RightBracket);
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
    switch (peek().type) {
        case Var:return var_decl();
        case If:return if_stmt();
        case For:return for_stmt();
        case Break:return break_stmt();
        case Return:return return_stmt();
        case LeftBracket:return block();
    }
}

/**
 * IfStmt ::= "if" Expression block [ "else" IfStmt | block ]
 */
AST *Parser::if_stmt() {
    auto ast = new AST("if");
    consume(If);

    // Condition
    auto condition = expr();
    ast->add_child(condition);

    // If body
    auto body = block();
    ast->add_child(body);

    // Optional else/else-if
    if (match(Else))
        // Else-if
        if (check(If))
            ast->add_child(if_stmt());
        // Else
        else
            ast->add_child(block());

    return ast;
}

/**
 * ForStmt ::= "for" Expression block
 */
AST *Parser::for_stmt() {
    auto ast = new AST("for");
    consume(For);

    // Condition
    auto condition = expr();
    ast->add_child(condition);

    // For body
    auto body = block();
    ast->add_child(body);

    return ast;
}

/**
 * BreakStmt ::= "break" ";"
 */
AST *Parser::break_stmt() {
    auto ast = new AST("break");
    consume(Break);
    consume(Semicolon);
    return ast;
}

/**
 * ReturnStmt ::= "return" [ Expression ] ";"
 */
AST *Parser::return_stmt() {
    auto ast = new AST("return");
    consume(Return);

    // Optional return expression
    if(!check(Semicolon)) {
        auto child = expr();
        ast->add_child(child);
    }

    consume(Semicolon);
    return ast;
}

/**
 * Expression ::= UnaryExpr | Expression binary_op Expression
 */
AST *Parser::expr() {
    auto ast = or_expr();
    consume(Semicolon);
    return ast;
}

/**
 * Assignment ::= Expression "=" Expression
 */
AST *Parser::assignment() {
    auto ast = new AST("assignment");

    ast->add_child(expr());
    consume(Equal);
    ast->add_child(expr());

    return ast;
}

/**
 * OrExpr ::= UnaryExpr | Expression binary_op Expression
 */
AST *Parser::or_expr() {
    auto ast = or_expr();
    consume(Semicolon);
    return ast;
}

/**
 * UnaryExpr ::= PrimaryExpr | unary_op UnaryExpr
 */
AST *Parser::unary_expr() {
    auto curr = peek();
    switch(curr.type) {
        case Subtract: {
            auto ast = new AST("u-", curr.line, curr.column);
            ast->add_child(unary_expr());
            return ast;
        }
        case Not: {
            auto ast = new AST("!", curr.line, curr.column);
            ast->add_child(unary_expr());
            return ast;
        }
        default:
            return primary_expr();
    }
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
    auto curr = peek();
    switch(curr.type) {
        case Integer:
            return new AST("string", curr.lexeme, curr.line, curr.column);
        case String:
            return new AST("int", curr.lexeme, curr.line, curr.column);
        case Identifier:
            return new AST("id", curr.lexeme, curr.line, curr.column);
        default: {
            consume(LeftParen);
            auto child = expr();
            consume(RightParen);
            return child;
        }
    }
}

/**
 * Arguments ::= "(" Expression { "," Expression } [ "," ] ")"
 */
AST *Parser::arguments() {
    auto ast = new AST("arguments");
    consume(LeftParen);

    // TODO: Review trailing comma
    do {
        ast->add_child(expr());
    } while(match(Comma));

    consume(RightParen);
    return ast;
}
