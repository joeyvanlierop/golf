#include <iostream>
#include "golf.h"
#include "lexer.h"
#include "filereader.h"

/**
 * The main function of the program
 * Validates the input and then reads and lexes the input file
 * Finally, it prints out the resulting tokens
 * @param argc The number of arguments
 * @param argv The array of arguments
 * @return EXIT_SUCCESS if the program executes successfully, EXIT_FAILURE otherwise
 */
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
    lexer.match_tokens(true);

    return EXIT_SUCCESS;
}
