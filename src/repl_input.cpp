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

    auto open_braces = 0;
    if (Input::data.ends_with('{'))
        open_braces = 1;

    while(open_braces > 0) {
        Input::data.append("\n");
        std::cout << "... " << std::flush;
        Input::data.append(read_line());

        if (Input::data.ends_with('{'))
            open_braces = 1;
        if (Input::data.ends_with('}'))
            open_braces -= 1;
    }
}





