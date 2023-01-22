#include "parser.h"
#include <iostream>

Parser::Parser() {
    this->lexer = new Lexer();
}

Node Parser::Parse(std::string input) {
    this->input = input;
    this->lexer->Init(this->input);

    lookahead = lexer->GetNextToken();

    return this->Program();
}

Node Parser::Program() {
    if(lookahead->type == "NUMBER")
        return NumericLiteral();

    throw std::runtime_error("Unexpected something");
}

NumericLiteralNode Parser::NumericLiteral() {
    auto token = Eat("NUMBER");
    return {
            .value = std::stoi(token.value)
    };
}

Token Parser::Eat(std::string tokenType) {
    auto token = lookahead;

    if(!token)
        throw std::runtime_error("Unexpected end of input, expected:" + tokenType);

    if(token->type != tokenType)
        throw std::runtime_error("Unexpected token: " + token->type + ", expected:" + tokenType);

    this->lookahead = lexer->GetNextToken();

    return token.value();
}
