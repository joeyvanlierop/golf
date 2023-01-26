#include "lexer.h"
#include <iostream>
#include <format>

Lexer::Lexer(const std::string &input) : input(input) {}

bool Lexer::is_at_end() {
    return current >= input.length();
}

char Lexer::advance() {
    return input[current++];
}

std::vector<Token> Lexer::match_tokens() {

    while (!is_at_end()) {
        start = current;
        auto token = match_token();
        if (token.has_value()) tokens.push_back(token.value());
    }
    tokens.push_back(create_token(Eof, current, current));

    return tokens;
}

std::optional<Token> Lexer::match_token() {
    char c = advance();
    switch (c) {
        // Whitespace
        case ' ':
        case '\r':
        case '\t':
            return std::nullopt;

        // Newline
        case '\n':
            line++;
            if(tokens.size() > 0)
                for(auto valid : {Identifier , Integer , String , Break , Return , RightParen , RightBracket})
                    if(tokens.back().type == valid)
                        return create_token(Semicolon, "\\n");
            return std::nullopt;

        // Single character
        case '{':
            return create_token(LeftBracket);
        case '}':
            return create_token(RightBracket);
        case '(':
            return create_token(LeftParen);
        case ')':
            return create_token(RightParen);
        case ';':
            return create_token(Semicolon);
        case ',':
            return create_token(Comma);
        case '+':
            return create_token(Add);
        case '*':
            return create_token(Multiply);

        // Either
        case '!':
            return create_token(either('=', NotEqual, Not));
        case '=':
            return create_token(either('=', EqualEqual, Equal));
        case '>':
            return create_token(either('=', GreaterEqual, Greater));
        case '<':
            return create_token(either('=', LessEqual, Less));

        // Binary
        case '&':
            if (match('&'))
                return std::nullopt;
            else
                return create_token(Error, "bitwise AND not supported");
        case '|':
            if (match('|'))
                return std::nullopt;
            else
                return create_token(Error, "bitwise OR not supported");

        // Comment
        case '/':
            if (match('/')) {
                while (!is_at_end() && peek() != '\n')
                    advance();
                return std::nullopt;
            } else {
                return create_token(Divide);
            }

        // String literal
        case '"':
            while (!is_at_end() && peek() != '"') {
                if (peek() == '\n')
                    return create_token(Error, "string contains newline");
                advance();
            }

            if (is_at_end())
                return create_token(Error, "unterminated string");

            advance();
            return create_token(String, start + 1, current - 1);

        // Non-trivial
        default:
            // Integer literal
            if (is_digit(c))
                return number();

            // Identifier
            else if (is_alpha(c))
                return identifier();

            // Unknown
            else
                return create_token(Warning, std::format("unknown character '{}'", c));
    }
}

Token Lexer::identifier() {
    while (!is_at_end() && is_alphanumeric(peek()))
        advance();

    auto lexeme = input.substr(start, (current - start));
    if (Keywords.count(lexeme))
        return create_token(Keywords[lexeme]);
    return create_token(Identifier);
}

Token Lexer::number() {
    while (!is_at_end() && is_digit(peek()))
        advance();
    return create_token(Integer);
}

Token Lexer::create_token(TokenType token_type) {
    return create_token(token_type, start, current);
}

Token Lexer::create_token(TokenType token_type, int start, int end) {
    return Token(token_type, input.substr(start, (end - start)), line);
}

Token Lexer::create_token(TokenType token_type, std::string lexeme) {
    return Token(token_type, lexeme, line);
}

bool Lexer::match(char expected) {
    if (peek() != expected)
        return false;

    advance();
    return true;
}

TokenType Lexer::either(char expected, TokenType matched, TokenType unmatched) {
    return match(expected) ? matched : unmatched;
}

char Lexer::peek() {
    return input.at(current);
}


bool Lexer::is_digit(char c) {
    return c >= '0' && c <= '9';
}

bool Lexer::is_alpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

bool Lexer::is_alphanumeric(char c) {
    return is_alpha(c) || is_digit(c);
}

