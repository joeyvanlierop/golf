#pragma once

#include "ast.h"
#include "token.h"
#include "file_input.h"
#include "input.h"
#include <vector>
#include <set>
#include <optional>

class Parser {
public:
    Parser(Input *input, std::vector<Token> tokens);
    AST* parse(bool verbose);

private:
    int current = 0;
    Input *input;
    std::vector<Token> tokens;

    bool is_at_end();
    Token advance();
    Token peek();
    Token previous();
    Token consume(TokenType type);
    Token consume(TokenType type, const std::string &error_message);
    bool check(TokenType type);
    bool match(TokenType expected);
    AST* decl();
    AST* var_decl(bool global);
    AST* func_decl();
    AST* func_sig();
    AST* block();
    AST* stmt();
    AST* if_stmt();
    AST* for_stmt();
    AST* break_stmt();
    AST* return_stmt();
    AST* expr_stmt();
    AST* expr();
    AST *assignment();
    AST *or_expr();
    AST *and_expr();
    AST *rel_expr();
    AST *add_expr();
    AST *mul_expr();
    AST *unary_expr();
    AST *func_call();
    AST *operand();
};
