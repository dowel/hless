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
	Chunk(const std::string& name, 
		  Readable& file,
		  u64 start);

	MetaLine& get_line(s64 index) { return _lines.at(index + _first_line_index); }
	u64 get_lines_count() const { return _lines.size(); }
	u64 get_length() const { return _length; }
	u64 get_start_offset() const { return _start_offset; }
	u64 get_end() const { return _start_offset + _length; }
	s64 get_index_at_offset(u64 offset);
	
	void split_lines(char* buffer, u32 length);
	void split_lines_reversed(char* buffer, u32 length);
	void grow_up(bool eol_aligned);
	void grow_up(u64 how_much, bool eol_aligned);
	void grow_down();
	void grow_down(u64 how_much);

	u64 get_absolute_line_index(s64 index) { return index + _first_line_index; }
	s64 get_first_line_index() { return -_first_line_index; }
	s64 get_last_line_index() { return _lines.size() - _first_line_index - 1; }

	Readable& get_file() { return _file; }

	std::string get_name() { return _name; }

	bool adjacent_chunks(const Chunk* other)
	{
		return (get_end() == other->get_start_offset()) || 
			(other->get_end() == get_start_offset());
	}

	friend std::ostream& operator<<(std::ostream& os, Chunk& chunk);
	friend std::ostream& operator<<(std::ostream& os, Chunk* chunk);
	friend std::ostream& operator<<(std::ostream& os, const Chunk& chunk);
	friend std::ostream& operator<<(std::ostream& os, const Chunk* chunk);

private:
	std::string _name;
	Readable& _file;

	u64 _start_offset;
	u64 _length;

	std::deque<MetaLine> _lines;
	// first line index is the index of the first line in the chunk. Normally, when chunk is created
	// this number is 0. If chunk grows down it remains 0. If chunk grows up, this number increases.
	// This is needed for two reasons: 1. generating name of the line. Usually it consists of chunk name
	// and line number. So, if chunk name is END, line name END+10 doesn't make sense. So, for chunks
	// that grow up we have to be able to calculate negative index of the line in the chunk. To do that,
	// we have to know first line of the chunk in the line.
	// Another and more important reason for this is iterators. Iterator has line number inside. When
	// growing chunk up, line number in the iterator will become invalid. To avoid this, when growing
	// up, line numbers of the newly added lines is in the negative range.
	u64 _first_line_index;
};

std::ostream& operator<<(std::ostream& os, Chunk& chunk);
std::ostream& operator<<(std::ostream& os, Chunk* chunk);

#endif

