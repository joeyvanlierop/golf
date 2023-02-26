#pragma once

#include "ast.h"
#include "token.h"
#include "filereader.h"
#include <vector>
#include <set>
#include <optional>

static std::set<TokenType> DECL_START({
    Var,
    Func,
});

class Parser {
public:
    Parser(FileReader *filereader, std::vector<Token> tokens);
    AST* parse();

private:
    int current = 0;
    FileReader *filereader;
    std::vector<Token> tokens;

    bool is_at_end();
    Token advance();
    Token peek();
    Token consume(TokenType type);
    bool check(TokenType type);
    AST* decls();
    AST* decl();
    AST* var_decl();
    AST* var_spec();
    AST* func_decl();
    AST* func_sig();
    AST* block();
    AST* stmts();
    AST* stmt();
    AST* if_stmt();
    AST* for_stmt();
    AST* break_stmt();
    AST* return_stmt();
    AST* expr();
    AST* unary_expr();
    AST* primary_expr();
    AST* operand();
    AST* arguments();
    AST* assignment();
};
