#include <string>
#include <optional>
#include <regex>
#include <iostream>
#include <iomanip>
#include "token.h"

//struct Token {
//    std::string type;
//    std::string value;
//    int line = 0;
//    int column = 0;
//
//    friend std::ostream& operator<<(std::ostream &os, const Token& t) {
//        os << std::setw(8) << std::left  << t.type << " [" << t.value << "] " << "@ (" << t.line << ", " << t.column << ")";
//        return os;
//    }
//};

static std::pair<std::string, TokenType> Spec[] =  {
        // Whitespace
        {"^\\s+", Unknown},

        // Single-line comments
        {"^\\/\\/.*", Unknown},

        // Keywords
        {"^break\\b", Break},
        {"^else\\b", Else},
        {"^for\\b", For},
        {"^func\\b", Func},
        {"^if\\b", If},
        {"^return\\b", Return},
        {"^var\\b", Var},

        // Double-quoted strings
        {"^\"[^\"]*\"", String},

        // Integer literals
        {"^[0-9]+\\b", Integer},

        // Identifiers
        {"^[a-zA-Z_]\\w*\\b", Identifier},

        // Algebraic operators
        {"^\\+$", Add},     // Addition
        {"^-$", Subtract},       // Subtraction
        {"^\\*$", Multiply},     // Multiplication
        {"^/$", Divide},       // Division
        {"^%$", Modulo},       // Modulus

        // Relational operators
        {"^>", Greater},        // Greater than
        {"^>=", GreaterEqual},      // Greater than or equals
        {"^<", Less},        // Less than
        {"^<=", LessEqual},      // Less than or equals
        {"==", Equal},       // Equals
        {"!=", NotEqual},       // Not equals

        // Logical operators
        {"^&&", And},      // AND
        {"^\\|\\|", Or},  // OR
        {"^!", Not},        // NOT
        {"^=", Equal},        // Assignment
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
