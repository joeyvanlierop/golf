#include <iostream>
#include "golf.h"
#include "lexer.h"
#include "filereader.h"

int main(int argc, char* argv[]) {
    // Validate input
    if (argc != 2)
    {
        printf("Usage: %s [filename]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Read input
    FileReader reader(argv[1]);

    // Lex input
    Lexer lexer(&reader);

    // Debugging
    auto tokens = lexer.match_tokens();
    for(auto token : tokens)
        std::cout << token << std::endl;

    return EXIT_SUCCESS;
}
