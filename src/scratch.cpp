#include <set>
#include <string>
#include <vector>

#include "scratch.h"

std::vector<Scratch> scratch_table = {};

Scratch scratch_create(int reg) {
	Scratch s;
	s.reg = reg;
	return s;
}

void scratch_init() {
	for(int i = 7; i >= 0; i--) {
		auto s = scratch_create(i);
		scratch_table.push_back(s);
	}
}


int scratch_alloc() {
	if(scratch_table.empty()) {
		printf("error: out of registers\n");
		exit(1);
	}

	auto s = scratch_table.back();
	scratch_table.pop_back();
	return s.reg;
}

//void scratch_display_regs() {
//	struct scratch * curr = scratch_top;
//	while(curr) {
//		printf("reg name: %s  reg num: %d  reg used %d\n", curr->name, curr->reg, curr->using);
//		curr = curr->next;
//	}
//}

void scratch_free(int r) {
	auto s = scratch_create(r);
	scratch_table.push_back(s);
}
