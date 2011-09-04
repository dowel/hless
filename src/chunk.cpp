#include "chunk.h"
#include "log.h"
#include "config.h"

static __attribute__((unused)) const char* MODULE_NAME = "chunk";

Chunk::Chunk(std::string name, Readable& file, u32 chunk_index, u64 start)
	: _name(name)
	, _file(file)
	, _chunk_index(chunk_index)
	, _start_offset(start)
	, _length(0)
	, _next(0)
	, _prev(0)
{ }

void Chunk::split_lines(char* buffer, u32 length)
{
	u32 i = 0;
	u32 old_i = 0;
	while (i < length) {
		if (buffer[i] == '\n') {
			MetaLine line(_start_offset + _length + old_i, i - old_i + 1);
			Log2("Adding line " << line);
			_lines.push_back(line);
			old_i = i + 1;
		}
		i++;
	}
	_length += old_i;
}

void Chunk::split_lines_reversed(char* buffer, u32 length)
{
	u32 i = length - 1;
	while (i > 0) {
		i--;
		if (buffer[i] == '\n') {
			MetaLine line(_start_offset + _length + i, length - i);
			Log2("Adding line " << line);
			_lines.push_front(line);
			length = i;
		}
	}

	MetaLine line(_start_offset + _length, length);
	Log2("Adding line " << line);
	_lines.push_front(line);
}

void Chunk::grow_up()
{
	Log1("Asked to grow chunk " << this << " up");
	Log2("Chunk before growing: " << this);
	if (_start_offset == 0) {
		Log2("Chunk starts at the beginnig of the file");
		return;
	}

	if (_prev && _start_offset == _prev->_start_offset + _prev->_length) {
		Log2("Chunk beginning is adjacent to the end of previous chunk");
		return;
	}

	u64 start_from;
	u64 how_much;
	u64 rel_start;
	if (_prev) {
		rel_start = _prev->_start_offset + _prev->_length;
	} else {
		rel_start = 0;
	}

	if (_start_offset - rel_start <= Config::chunk_grow_size) {
		start_from = rel_start;
		how_much = _start_offset - rel_start;
	} else {
		start_from = _start_offset - Config::chunk_grow_size;
		how_much = Config::chunk_grow_size;
	}

	Log3("Chunk growing by " << how_much << " starting from " << start_from);

	char buffer[how_much];
	u32 n = 0;
	_file.read(how_much, start_from, buffer);

	/**
	 * Making sure buffer starts at the beginning of the line. If buffer adjacent to 
	 * end of previous chunk, then we're ok. Otherwise, we have to find next available 
	 * end of line and start from it. 
	 */
	if ((_prev && (_prev->_start_offset + _prev->_length != start_from)) || !_prev) {
		while ((buffer[n] != '\n') && (n < how_much)) {
			n++;
		}
		n++;
	}

	start_from += n;
	how_much -= n;
	_start_offset = start_from;

	split_lines_reversed(&buffer[n], how_much);
	Log2("Chunk after growing: " << this);
}

void Chunk::grow_down()
{
	Log1("Asked to grow chunk " << this << " down");
	Log2("Chunk before growing: " << this);

	if (_next && _next->_start_offset == _start_offset + _length) {
		Log2("Chunk end is adjacent to the beginning of next chunk");
		return;
	}

	u64 how_much = Config::chunk_grow_size;
	if (_next && (_next->_start_offset - (_start_offset + _length) < how_much)) {
		how_much = _next->_start_offset - (_start_offset + _length);
	}

	char buffer[how_much];
	u32 n =_file.read(Config::chunk_grow_size, _start_offset + _length, buffer);
	split_lines(buffer, n);
	Log2("Chunk after growing: " << this);
}

std::ostream& operator<<(std::ostream& os, Chunk& chunk)
{
	if (chunk._name.length() == 0) {
		os << "[Chunk: first chunk, ";
	} else {
		os << "[Chunk: " << chunk._name << ", ";
	}
	os << chunk._start_offset << "-";
	os << chunk._start_offset + chunk._length << "(+" << chunk._length << "), ";
	os << chunk._lines.size();
	os << "]";
	return os;
}

std::ostream& operator<<(std::ostream& os, Chunk* chunk)
{
	os << *chunk;
	return os;
}

