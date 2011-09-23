#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "types.h"

class Config
{
public:
	static const u64 chunk_grow_size = 1024;
	static const u32 status_bar_height = 2;
};

#endif
