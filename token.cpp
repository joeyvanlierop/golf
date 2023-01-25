#include "token.h"
#include <iomanip>

std::ostream &operator<<(std::ostream &os, TokenType tokenType) {
    switch (tokenType) {
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
        case TokenType::Unknown :
            return os << "unknown";
    }
    return os << static_cast<std::uint16_t>(tokenType);
}

std::string to_string(TokenType const& tokenType)
{
    std::ostringstream ss;
    ss << tokenType;
    return std::move(ss).str();
}

std::ostream &operator<<(std::ostream &os, Token token) {
    return os << std::setw(8) << std::left << token.type << " [" << token.value << "] " << "@ (" << token.line << ", "
       << token.column << ")";
}
