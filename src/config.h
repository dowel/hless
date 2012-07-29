#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "types.h"

class Config
{
public:
	// Specifies how many bytes to read every time one of the buffer chunks grow either up or down.
	static const u64 chunk_grow_size = 1024;

	// Specifies height of the status bar
	static const u32 status_bar_height = 2;

	// When asked to go to specific offset in the buffer, this variable specifies maximum distance that
	// allows growing existing chunks vs. creating a new chunk.
	static const u64 max_distance_to_grow_chunk = 1024;
};

#endif
