#ifndef __CACHE_H__
#define __CACHE_H__

#include "file.h"

class Cache : public Readable
{
public:
	Cache(Readable& readable) : _readable(readable) { }

	virtual u32 read(u32 how_much, u64 offset, char* buffer);

private:
	Readable& _readable;
};

#endif
