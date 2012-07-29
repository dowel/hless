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
		  u64 start,
		  Chunk* next,
		  Chunk* prev);

	MetaLine& get_line(s64 index) { return _lines.at(index + _first_line_index); }
	u64 get_lines_count() { return _lines.size(); }
	Chunk* get_next() { return _next; }
	Chunk* get_prev() { return _prev; }
	u64 get_length() { return _length; }
	u64 get_start_offset() { return _start_offset; }
	u64 get_end() { return _start_offset + _length; }
	
	void split_lines(char* buffer, u32 length);
	void split_lines_reversed(char* buffer, u32 length);
	void grow_up();
	void grow_down();

	u64 get_absolute_line_index(s64 index) { return index + _first_line_index; }
	s64 get_first_line_index() { return -_first_line_index; }
	s64 get_last_line_index() { return _lines.size() - _first_line_index - 1; }

	Readable& get_file() { return _file; }

	std::string get_name() { return _name; }

	friend std::ostream& operator<<(std::ostream& os, Chunk& chunk);
	friend std::ostream& operator<<(std::ostream& os, Chunk* chunk);

private:
	std::string _name;
	Readable& _file;
	u32 _chunk_index;

	u64 _start_offset;
	u64 _length;

	std::deque<MetaLine> _lines;
	// first line index is the index of the first line in the chunk. Normally, when chunk is created
	// this number is 0. If chunk grows down it remains 0. If chunk grows up, this number increases.
	// This is needed for two reasons: 1. generating name of the line. Usually it consists of chunk name
	// and line number. So, if chunk name is END, line name END+10 doesn't make sense. So, for chunks
	// that grow up we have to be able to calculate negative index of the line in the chunk. To do that,
	// we have to know first line of the chunk in the line. Another reason for this is iterators.
	// Another and more important reason for this is iterators. Iterator has line number inside. When
	// growing chunk up, line number in the iterator will become invalid. To avoid this, when growing
	// up, line numbers of the newly added lines is in the negative range.
	u64 _first_line_index;

	Chunk* _next;
	Chunk* _prev;
};

std::ostream& operator<<(std::ostream& os, Chunk& chunk);
std::ostream& operator<<(std::ostream& os, Chunk* chunk);

#endif

