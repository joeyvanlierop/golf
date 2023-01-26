#include <string>
#include <iostream>
#include "golf.h"
#include "lexer.h"

int main() {
    std::string input = R"(
"test"
"unterminated
if {
    var hello = 42;
}
^
"another^)";
//std::string input = R"(42 abc break "123")";
    Lexer lexer = Lexer(input);
    auto tokens = lexer.match_tokens();
    for(auto token : tokens) {
        if(token.type == Error || token.type == Warning)
            std::cerr << token << std::endl;
        else
            std::cout << token << std::endl;
    }
}
