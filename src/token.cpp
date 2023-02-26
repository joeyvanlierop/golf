#include "token.h"
#include <iomanip>

/**
 * Constructs a new `Token` object with the given type, lexeme, line and column.
 * @param type the type of token
 * @param lexeme the string representation of the token
 * @param line the line number in the source code where the token is found
 * @param column the column number in the source code where the token is found
 */
Token::Token(TokenType type, const std::string &lexeme, int line, int column) : type(type), lexeme(lexeme), line(line),
                                                                                column(column) {}
/**
 * Overloading the output stream operator to print the `TokenType`
 * Formats as a human-readable string
 * @param os the output stream
 * @param tokenType the token type to print
 * @return the updated output stream
 */
std::ostream &operator<<(std::ostream &os, TokenType tokenType) {
    switch (tokenType) {
        case TokenType::LeftBracket :
            return os << "{";
        case TokenType::RightBracket :
            return os << "}";
        case TokenType::LeftParen :
            return os << "(";
        case TokenType::RightParen :
            return os << ")";
        case TokenType::Semicolon :
            return os << ";";
        case TokenType::Colon :
            return os << ":";
        case TokenType::Comma :
            return os << ",";
        case TokenType::Or :
            return os << "||";
        case TokenType::And :
            return os << "&&";
        case TokenType::Not :
            return os << "!";
        case TokenType::NotEqual :
            return os << "!=";
        case TokenType::Equal :
            return os << "=";
        case TokenType::EqualEqual :
            return os << "==";
        case TokenType::Greater :
            return os << ">";
        case TokenType::GreaterEqual :
            return os << ">=";
        case TokenType::Less :
            return os << "<";
        case TokenType::LessEqual :
            return os << "<=";
        case TokenType::Add :
            return os << "+";
        case TokenType::Subtract :
            return os << "-";
        case TokenType::Multiply :
            return os << "*";
        case TokenType::Divide :
            return os << "/";
        case TokenType::Modulo :
            return os << "%";
        case TokenType::Identifier :
            return os << "id";
        case TokenType::String :
            return os << "string";
        case TokenType::Integer :
            return os << "int";
        case TokenType::Break :
            return os << "break";
        case TokenType::Else :
            return os << "else";
        case TokenType::For :
            return os << "for";
        case TokenType::Func :
            return os << "func";
        case TokenType::If :
            return os << "if";
        case TokenType::Return :
            return os << "return";
        case TokenType::Var :
            return os << "var";
        case TokenType::Eof :
            return os << "eof";
    }
    return os << static_cast<std::uint16_t>(tokenType);
}

/**
 * Overloading the output stream operator to print the `Token`.
 * Output format: <token type> [<lexeme>] @ (<line>, <column>)
 * @param os the output stream
 * @param token the token to print
 * @return the updated output stream
 */
std::ostream &operator<<(std::ostream &os, Token token) {
    return os << std::setw(8) << std::left << token.type << " [" << token.lexeme << "] " << "@ (" << token.line << ", "
              << token.column << ")";
}

