#pragma once

#include <string>

class Input {
public:
    std::string data;
    std::string name;

    Input(const std::string& name);
    virtual void read() = 0;
    virtual std::string get_line(int line_number);
};

