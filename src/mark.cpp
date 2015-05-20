
#include "mark.h"

bool operator<(const Mark& first, const Mark& second)
{
	return first._it < second._it;
}

std::ostream& operator<<(std::ostream& os, Mark& mark)
{
	os << "[Mark: " << mark._it << ", color " << mark._color << "]";
	return os;
}

