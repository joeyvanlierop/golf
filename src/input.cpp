#include <iostream>
#include <sstream>
#include "input.h"

Input::Input(const std::string& name) : name(name) {}

std::string Input::get_line(int line_number) {
    std::istringstream data_stream(data);
    std::string line;
    for(int i = 0; i < line_number; i++)
        std::getline(data_stream, line);
    return line;
}
