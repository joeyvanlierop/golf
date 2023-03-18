#pragma once

#include <string>
#include "input.h"

class ReplInput : public Input {
public:
    ReplInput();
    void read() override;
};
