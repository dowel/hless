#include "config.h"

// Specifies how many bytes to read every time one of the buffer chunks grow either up or down.
const u64 Config::chunk_grow_size = 1024;

// Specifies height of the status bar
const u32 Config::status_bar_height = 2;

// When asked to go to specific offset in the buffer, this variable specifies maximum distance that
// allows growing existing chunks vs. creating a new chunk.
const u64 Config::max_distance_to_grow_chunk = 1024;

