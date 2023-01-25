#include <set>
#include <string>
#include <iostream>

const std::set<std::string> Keywords = {
        "break",
        "else",
        "for",
        "func",
        "if",
        "return",
        "var",
};

enum TokenType {
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
    Unknown,
};

std::ostream &operator<<(std::ostream &os, TokenType tokenType);

std::string to_string(TokenType const& tokenType);

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

std::ostream &operator<<(std::ostream &os, Token token);

