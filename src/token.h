#pragma once

#include <map>
#include <string>
#include <iostream>

enum TokenType {
    // Punctuation
    LeftBracket,
    RightBracket,
    LeftParen,
    RightParen,
    Semicolon,
    Colon,
    Comma,

    // Binary operators
    Or,
    And,

    // Relational operators
    Not,
    NotEqual,
    Equal,
    EqualEqual,
    Greater,
    GreaterEqual,
    Less,
    LessEqual,

    // Arithmetic operators
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulo,

    // Literals
    Identifier,
    String,
    Integer,

    // Keywords
    Break,
    Else,
    For,
    Func,
    If,
    Return,
    Var,

    // Other
    Eof,
};

static std::map<std::string, TokenType> Keywords = {
        {"break", Break},
        {"else", Else},
        {"for", For},
        {"func", Func},
        {"if", If},
        {"return", Return},
        {"var", Var},
};

std::ostream &operator<<(std::ostream &os, TokenType tokenType);

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;

    Token(TokenType type, const std::string &lexeme, int line, int column);
};

std::ostream &operator<<(std::ostream &os, Token token);

