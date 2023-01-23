#include "parser.h"

Parser::Parser() {
    this->lexer = new Lexer();
}

Node Parser::Parse(std::string input) {
    this->input = input;
    this->lexer->Init(this->input);

    lookahead = lexer->GetNextToken();

    Node n;
    while(lookahead.has_value())
        n = Program();
    return n;
}

Node Parser::Program() {
//    if(lookahead->type == "int")
//        return IntegerLiteral();
//    else if(lookahead->type == "string")
//        return StringLiteral();
//    else if(lookahead->type == "id")
//        return IdentifierLiteral();
    Eat(lookahead->type);
    return DummyNode();

    throw std::runtime_error("Unexpected kind: " + lookahead->type);
}

Token Parser::Eat(std::string tokenType) {
    auto token = lookahead;

    if(!token)
        throw std::runtime_error("Unexpected end of input, expected:" + tokenType);

    if(token->type != tokenType)
        throw std::runtime_error("Unexpected token: " + token->type + ", expected:" + tokenType);

    // Advance lookahead
    this->lookahead = lexer->GetNextToken();

    // Debug, delete later
    std::cout << token.value() << std::endl;

    return token.value();
}

Node Parser::DummyNode() {
    return {};
}

NumericLiteralNode Parser::IntegerLiteral() {
    auto token = Eat("int");
    return {
            .value = std::stoi(token.value)
    };
}

StringLiteralNode Parser::StringLiteral() {
    auto token = Eat("string");
    return {
            .value = token.value
    };
}

IdentifierLiteralNode Parser::IdentifierLiteral() {
    auto token = Eat("id");
    return {
            .value = token.value
    };
}