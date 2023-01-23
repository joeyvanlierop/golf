#include "lexer.h"
#include <iostream>

void Lexer::Init(std::string input) {
    this->input = input;
    this->cursor = 0;
}

bool Lexer::HasMoreTokens() {
    return cursor < input.length();
}

std::optional<Token> Lexer::GetNextToken() {
    // We are out of tokens
    if(!HasMoreTokens()) {
        return std::nullopt;
    }

    // Slice the input to begin at the cursor
    auto currentInput = input.substr(cursor);

    // Iterate over our specification rules
    for(auto [regexp, tokenType] : Spec) {
        auto tokenValue = Match(currentInput, regexp);

        // Couldn't match this rule, continue
        if(!tokenValue)
            continue;

        // Matched kind is an error, halt immediately
        if(tokenType == "error"){
            std::cout << "error: " << currentInput[0] << "'" << std::endl;
        }
            return GetNextToken();

        // Matched kind should be ignored, return next match
        if(tokenType == "ignore")
            return GetNextToken();

        // We return the token
        return (Token){
            .type = tokenType,
            .value = tokenValue.value(),
        };
    }

    cursor += 1;
    std::cout << "warning: skipping unknown character '" << currentInput[0] << "'" << std::endl;
    return GetNextToken();
}

std::optional<std::string> Lexer::Match(std::string input, std::string regexp) {
    std::smatch match;
    if(!std::regex_search(input, match,std::regex(regexp)))
        return std::nullopt;

    cursor += match[0].length();
    return match[0];
}