#include <iostream>

#ifdef _WIN32

#include <Windows.h>

#elif __linux__
#include <sys/ioctl.h>
#endif

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
        std::string more;
        std::cout << "... " << std::flush;
        std::getline(std::cin, more);
        Input::data.append(more);

        if (Input::data.ends_with('{'))
            open_braces = 1;
        if (Input::data.ends_with('}'))
            open_braces -= 1;
    }
}

bool ReplInput::is_shift_pressed() {
#ifdef _WIN32
    return GetKeyState(VK_SHIFT) < 0;
#elif __linux__
    char shift_state;
    shift_state = 6;
    ioctl(0, TIOCLINUX, &shift_state);
    std::cout << shift_state << std::endl;
    return shift_state = 0;
#else
    return false;
#endif
}





