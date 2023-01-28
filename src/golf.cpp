#include <iostream>
#include "golf.h"
#include "lexer.h"
#include "filereader.h"

int main(int, char* argv[]) {
    // Read input
    FileReader reader(argv[1]);

    // Lex input
    Lexer lexer(&reader);

    // Debugging
    auto tokens = lexer.match_tokens();
    for(auto token : tokens) {
        if(token.type == Error || token.type == Warning)
            std::cerr << token << std::endl;
        else
            std::cout << token << std::endl;
    }

    return EXIT_SUCCESS;
}
