#pragma once

#include <string>
#include <vector>
#include <optional>
#include "token.h"
#include "file_input.h"
#include "input.h"

class Lexer {
public:
    Lexer(Input* input);
    std::vector<Token> match_tokens(bool verbose);

private:
    int start = 0;
    int current = 0;
    int line = 1;
    int column = 1;
    Input *input;
    std::vector<Token> tokens;

    bool is_at_end();
    char advance();
    Token create_token(TokenType token_type);
    Token create_token(TokenType token_type, std::string lexeme);
    Token create_token(TokenType token_type, int start, int end);
    bool match(char expected);
    TokenType either(char expected, TokenType matched, TokenType unmatched);
    char peek();
    bool is_digit(char c);
    bool is_alpha(char c);
    bool is_alphanumeric(char c);
    Token number();
    Token identifier();
    std::optional<Token> infer_semicolon();
    std::optional<Token> newline();
    std::optional<Token> match_token();
};
