#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <termios.h>
#endif
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
    return GetAsyncKeyState(VK_SHIFT) & 0x8000;
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    bool shift = false;
    int nread;
    char c;
    do {
        nread = read(STDIN_FILENO, &c, 1);
        if (nread == 1 && c == '\e') {
            nread = read(STDIN_FILENO, &c, 1);
            if (nread == 1 && c == '[') {
                nread = read(STDIN_FILENO, &c, 1);
                if (nread == 1 && (c == '2' || c == '3')) {
                    nread = read(STDIN_FILENO, &c, 1);
                    if (nread == 1 && c == '~') {
                        shift = (c == '2');
                        break;
                    }
                }
            }
        }
    } while (nread == 1);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return shift;
#endif
}
