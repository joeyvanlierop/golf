#include <iostream>
#include "repl_input.h"
#include "lexer.h"

/**
 * Constructs a ReplInput object with a default name
 */
ReplInput::ReplInput() : Input("repl") {

}

/**
 * Reads from an interactive prompt
 * @return the user input
 */
std::string read_line() {
    std::string more;
    std::getline(std::cin, more);
    return more;
}

/**
 * Reads from an interactive prompt
 * Terminated by entring an empty line
 * @return the user input
 */
void ReplInput::read() {
    std::cout << ">>> " << std::flush;
    std::string line;
    while((line = read_line()).length() != 0) {
        Input::data.append(line);
        Input::data.append("\n");
        std::cout << "... " << std::flush;
    }
}





