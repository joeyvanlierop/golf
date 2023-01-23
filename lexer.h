#include <string>
#include <optional>
#include <regex>
#include <iostream>
#include <iomanip>

struct Token {
    std::string type;
    std::string value;
    int line = 0;
    int column = 0;

    friend std::ostream& operator<<(std::ostream &os, const Token& t) {
        os << std::setw(8) << std::left  << t.type << " [" << t.value << "] " << "@ (" << t.line << ", " << t.column << ")";
        return os;
    }
};

static std::pair<std::string, std::string> Spec[] =  {
        // Whitespace
        {"^\\s+", "ignore"},

        // Single-line comments
        {"^\\/\\/.*", "ignore"},

        // Keywords
        {"^break", "break"},
        {"^else", "else"},
        {"^for", "for"},
        {"^func", "func"},
        {"^if", "if"},
        {"^return", "return"},
        {"^var", "var"},

        // Integer literals
        {"^\\d+\\b", "int"},

        // Double-quoted strings
        {"^\"[^\"]*\"", "string"},

        // Identifiers
        {"^\\w+\\b", "id"},

        // Algebraic operators
        {"^\\+$", "+"},     // Addition
        {"^-$", "-"},       // Subtraction
        {"^\\*$", "*"},     // Multiplication
        {"^/$", "/"},       // Division
        {"^%$", "%"},       // Modulus

        // Relational operators
        {"^>", ">"},        // Greater than
        {"^>=", ">="},      // Greater than or equals
        {"^<", "<"},        // Less than
        {"^<=", "<="},      // Less than or equals
        {"==", "=="},       // Equals
        {"!=", "!="},       // Not equals

        // Logical operators
        {"^&&", "&&"},      // AND
        {"^\\|\\|", "||"},  // OR
        {"^!", "!"},        // NOT

        // Punctuation
        {"^\\(", "("},      // Left parenthesis
        {"^\\)", ")"},      // Right parenthesis
        {"^\\{", "{"},      // Left curly brace
        {"^\\}", "}"},      // Right curly brace
        {"^,", ","},        // Comma
        {"^\\.", "."},      // Dot
        {"^;", ";"},        // Semicolon
        {"^:", ":"},        // Colon
        {"^=", "="},        // Assignment
};

class Lexer {
private:
    std::string input;
    int cursor;
    bool HasMoreTokens();
    std::optional<std::string> Match(std::string regexp, std::string input);
public:
    void Init(std::string input);
    std::optional<Token> GetNextToken();
};
