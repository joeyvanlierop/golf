#include <string>
#include <vector>
#include <optional>
#include "token.h"

class Lexer {
public:
    Lexer(const std::string &input);
    std::optional<Token> match_token();
    std::vector<Token> match_tokens();

private:
    int start = 0;
    int current = 0;
    int line = 1;
    std::string input;

    bool is_at_end();
    char advance();
    Token create_token(TokenType token_type);
    Token create_token(TokenType token_type, int start, int end);
    bool match(char expected);
    TokenType either(char expected, TokenType matched, TokenType unmatched);
    char peek();
    bool is_digit(char c);
    bool is_alpha(char c);
    bool is_alphanumeric(char c);
    Token number();
    Token identifier();
};
