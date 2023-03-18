#pragma once

#include <string>
#include "input.h"

class FileInput : public Input {
public:
    FileInput(const std::string &filename);
    void read() override;
};
