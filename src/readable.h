#ifndef __READABLE_H__
#define __READABLE_H__

#include "types.h"

class Readable
{
public:
	virtual u32 read(u32 how_much, u64 offset, char* buffer) = 0;
	virtual u64 get_size() = 0;
};

#endif
