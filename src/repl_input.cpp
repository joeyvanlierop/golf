#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#include <xcb/xcb.h>
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
    static xcb_connection_t* conn = xcb_connect(nullptr, nullptr);
    if (!conn) return false;
    xcb_query_keymap_reply_t* reply = xcb_query_keymap_reply(conn, xcb_query_keymap(conn), nullptr);
    if (!reply) return false;
    const uint8_t* keys = xcb_query_keymap_keys(reply);
    const xcb_keysym_t shift_keysym = 0xffe1;
    const uint8_t shift_keycode = 54;
    bool is_shift_pressed = keys[shift_keycode / 8] & (1 << (shift_keycode % 8));
    xcb_disconnect(conn);
    free(reply);
    return is_shift_pressed;
#else
    return false;
#endif
}





