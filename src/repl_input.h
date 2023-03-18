#pragma once

#include <string>
#include "input.h"

class ReplInput : public Input {
private:
    bool is_shift_pressed();

public:
    ReplInput();
    void read() override;
};
