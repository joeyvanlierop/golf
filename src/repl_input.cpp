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
void ReplInput::read() {
    std::cout << ">>> " << std::flush;
    std::getline(std::cin, Input::data);
    while(is_shift_pressed()) {
        Input::data.append("\n");
        std::string more;
        std::cout << "... " << std::flush;
        std::getline(std::cin, more);
        Input::data.append(more);
    };
}

bool ReplInput::is_shift_pressed() {
#ifdef _WIN32
    return GetKeyState(VK_SHIFT) < 0;
#elif __linux__
    return ioctl(0, TIOCLINUX, &shift_state) == 0;
#else
    return false;
#endif
}





