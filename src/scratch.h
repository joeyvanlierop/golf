#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct Scratch {
	int reg;
	std::string to_string() {
		return "$s" + reg;
	}
};

Scratch scratch_create(int reg);
void scratch_init();
int scratch_alloc();
void scratch_free(int r);
void scratch_display_regs();