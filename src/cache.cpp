#include "cache.h"

static __attribute__((unused)) const char* MODULE_NAME = "cache";

u32 Cache::read(u32 how_much, u64 offset, char* buffer)
{
	return _readable.read(how_much, offset, buffer);
}

