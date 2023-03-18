#include <iostream>
#include <Windows.h>
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
void ReplInput::read() {
    std::cout << ">>> " << std::flush;
    std::getline(std::cin, Input::data);
    while((GetKeyState( VK_SHIFT ) & 0x8000) != 0) {
        Input::data.append("\n");
        std::string more;
        std::cout << "... " << std::flush;
        std::getline(std::cin, more);
        Input::data.append(more);
    };
}
