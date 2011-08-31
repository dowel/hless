#ifndef __CHUNK_H__
#define __CHUNK_H__

#include <iostream>
#include <string>
#include <deque>

#include "types.h"
#include "meta_line.h"
#include "readable.h"

/**
 * Class below is a container for lines. Purpose of this class is to allow buffer to contain sparse
 * portions of the file. Thus when going to the end of the file, there is no need to read entire
 * file - we can create a new chunk that points to the end of the file.
 * It also contains reference to file which can be used to read actual data.
 */
class Chunk
{
public:
	Chunk(std::string name, 
		  Readable& file,
		  u32 chunk_index,
		  u64 start);

	MetaLine& get_line(u64 index) { return _lines[index]; }
	u64 get_size() { return _lines.size(); }
	Chunk* get_next() { return _next; }
	Chunk* get_prev() { return _prev; }
	u64 get_length() { return _length; }
	u64 get_start() { return _start; }
	
	void split_lines(char* buffer, u32 length);
	void grow_up();
	void grow_down();

	friend std::ostream& operator<<(std::ostream& os, Chunk& chunk);
	friend std::ostream& operator<<(std::ostream& os, Chunk* chunk);

private:
	std::string _name;
	Readable& _file;
	u32 _chunk_index;
	u64 _start;

	u64 _length;
	std::deque<MetaLine> _lines;
	Chunk* _next;
	Chunk* _prev;
};

std::ostream& operator<<(std::ostream& os, Chunk& chunk);
std::ostream& operator<<(std::ostream& os, Chunk* chunk);

#endif

