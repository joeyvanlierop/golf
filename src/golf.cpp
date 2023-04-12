#include "golf.h"
#include "lexer.h"
#include "file_input.h"
#include "parser.h"
#include "input.h"
#include "repl_input.h"
#include "semantic.h"
#include "code_gen.h"

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

    // TODO: Make this not garbage
    bool interactive = std::string(argv[1]) == "repl";

    do {
        // Read input
        Input *input;
        if(interactive)
            input = new ReplInput();
        else
            input = new FileInput(argv[1]);
        input->read();

        // Lex input
        Lexer lexer(input);
        auto tokens = lexer.match_tokens(false);

        // Parse tokens
        Parser parser(input, tokens);
        auto ast = parser.parse(false);

        // Analyze syntax
        Semantic semantic(input, *ast);
        auto annotated_ast = semantic.analyze(false);

        // Generate code
		generate_code(ast);
    } while(interactive);

    return EXIT_SUCCESS;
}
