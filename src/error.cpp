#include <execinfo.h>
#include <cxxabi.h>

#include "error.h"

std::ostream& operator<<(std::ostream& os, const Backtrace& bt)
{
	os << std::hex << std::showbase;
	for (int i = 0; i < bt._n; i++) {
		os << bt._ptrs[i] << " ";
	}

	os << std::endl;
	os << std::dec << std::noshowbase;
	return os;
}

