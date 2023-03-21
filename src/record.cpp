#include <iostream>
#include <iomanip>

#include "record.h"

std::ostream &operator<<(std::ostream &os, const Record &record) {
	os << "[" << std::setw(15) << std::left << record.sig << "] [" << std::setw(8) << std::left << record.rt_sig
	   << "] [" << std::setw(5) << std::left << std::boolalpha << record.is_const << "] [" << std::setw(5)
	   << std::left << std::boolalpha << record.is_type << "]";
	return os;
}