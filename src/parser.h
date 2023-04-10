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
	std::shared_ptr<AST> parse(bool verbose);

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
    std::shared_ptr<AST> decl();
    std::shared_ptr<AST> var_decl(bool global);
    std::shared_ptr<AST> func_decl();
    std::shared_ptr<AST> func_sig();
    std::shared_ptr<AST> block();
    std::shared_ptr<AST> stmt();
    std::shared_ptr<AST> if_stmt();
    std::shared_ptr<AST> for_stmt();
    std::shared_ptr<AST> break_stmt();
    std::shared_ptr<AST> return_stmt();
    std::shared_ptr<AST> expr_stmt();
    std::shared_ptr<AST> expr();
    std::shared_ptr<AST> assignment();
    std::shared_ptr<AST> or_expr();
    std::shared_ptr<AST> and_expr();
    std::shared_ptr<AST> rel_expr();
    std::shared_ptr<AST> add_expr();
    std::shared_ptr<AST> mul_expr();
    std::shared_ptr<AST> unary_expr();
    std::shared_ptr<AST> func_call();
    std::shared_ptr<AST> operand();
};
