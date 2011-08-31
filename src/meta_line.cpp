#include "meta_line.h"

static __attribute__((unused)) const char* MODULE_NAME = "buffer";

std::ostream& operator<<(std::ostream& os, MetaLine& line)
{
	os << "[MetaLine: " << line.get_offset() << "-" << (line.get_offset() + line.get_length()) << "(+" << 
		line.get_length() << ")]";
	return os;
}

