#include <iostream>
#include "repl_input.h"

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
 * @return the user input
 */
void ReplInput::read() {
    std::cout << ">>> " << std::flush;
    Input::data = read_line();

    int open_braces;
    while((open_braces = Input::data.ends_with('{') ? 1 : 0) > 0) {
        Input::data.append("\n");
        std::cout << "... " << std::string(open_braces, '\t') << std::flush;
        Input::data.append(read_line());

        if (Input::data.ends_with('{'))
            open_braces += 1;
        if (Input::data.ends_with('}'))
            open_braces -= 1;
    }
}





