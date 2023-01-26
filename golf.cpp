#include <string>
#include <iostream>
#include "golf.h"
#include "lexer.h"

int main() {
    std::string input = R"(
42
4 213 ^
"abc"
var cole
^
Þ­¾ï
if {
    var test = 0;
}
^)";
//std::string input = R"(42 abc break "123")";
    Lexer lexer = Lexer(input);
    auto tokens = lexer.match_tokens();
    for(auto token : tokens) {
        std::cout << token << std::endl;
    }
}
