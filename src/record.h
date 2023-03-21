#pragma once

#include <string>

struct Record {
	std::string sig;
	std::string rt_sig;
	bool is_const;
	bool is_type;

	friend std::ostream& operator<<(std::ostream& os, const Record& record);
};
