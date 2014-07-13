#include "chunk.h"
#include "log.h"
#include "config.h"

static __attribute__((unused)) const char* MODULE_NAME = "chunk";

Chunk::Chunk(std::string name, Readable& file, u32 chunk_index, u64 start, Chunk* next, Chunk* prev)
	: _name(name)
	, _file(file)
	, _start_offset(start)
	, _length(0)
	, _first_line_index(0)
{
	if (_next) {
		_next->_prev = this;
	}

	if (_prev) {
		_prev->_next = this;
	}
}

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
	u32 buffer_length = length;
	u32 i = length - 1;
	u64 old_size = _lines.size();
	_start_offset -= length;
	while (i > 0) {
		i--;
		if (buffer[i] == '\n') {
			MetaLine line(_start_offset + i + 1, buffer_length - i - 1);
			Log2("Adding line " << line);
			_lines.push_front(line);
			buffer_length = i;
		}
	}

	MetaLine line(_start_offset, buffer_length);
	Log2("Adding line " << line);
	_lines.push_front(line);
	_length += length;
	_first_line_index += _lines.size() - old_size;
}

void Chunk::grow_down()
{
	grow_down(Config::chunk_grow_size);
}

void Chunk::grow_down(u64 how_much)
{
	Log1("Asked to grow chunk " << this << " down" << " by " << how_much);
	Log2("Chunk before growing: " << this);

	if (_start_offset + _length >= _file.get_size()) {
		Log1("Chunk reached end of file. Cannot grow anymore...");
		return;
	}

	char buffer[how_much];
	u32 n =_file.read(how_much, _start_offset + _length, buffer);
	split_lines(buffer, n);
	Log2("Chunk after growing: " << this);
}

void Chunk::append_other(Chunk* other)
{
	_length += other->_length;
	_lines.insert(_lines.end(), other._lines.begin(), other._lines.end());
}

std::ostream& operator<<(std::ostream& os, Chunk& chunk)
{
	if (chunk._name.length() == 0) {
		os << "[Chunk: first chunk, ";
	} else {
		os << "[Chunk: " << chunk._name << ", ";
	}
	os << "size " << chunk._start_offset << "-";
	os << chunk._start_offset + chunk._length << "(+" << chunk._length << "), ";
	os << "lines " << chunk._lines.size() << ", first " << chunk._first_line_index;
	os << "]";
	return os;
}

std::ostream& operator<<(std::ostream& os, Chunk* chunk)
{
	os << *chunk;
	return os;
}

