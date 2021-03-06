#include <algorithm>

#include "chunk.h"
#include "log.h"
#include "config.h"
#include "error.h"

static __attribute__((unused)) const char* MODULE_NAME = "chunk";

Chunk::Chunk(const std::string& name, Readable& file, u64 start)
	: _name(name)
	, _file(file)
	, _start_offset(start)
	, _length(0)
	, _first_line_index(0)
{
	Log1("Creating new chunk " << name << " at offset " << start << " from " << Backtrace());
}

s64 Chunk::get_index_at_offset(u64 offset)
{
	s64 index = 0;
	auto it = std::find_if(_lines.begin(), _lines.end(), [&offset, &index](const MetaLine& ml) -> bool 
	{
		index++;
		return (ml.get_offset() <= offset) && (offset < ml.get_end());
	});

	if (it != _lines.end()) {
		return _first_line_index + index;
	}
	return _first_line_index;
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

void Chunk::grow_up(bool eol_aligned)
{
	grow_up(Config::chunk_grow_size, eol_aligned);
}

void Chunk::grow_up(u64 how_much, bool eol_aligned)
{
	Log1("Asked to grow chunk " << this << " up" << " by " << how_much);
	Log2("Chunk before growing: " << this);
	if (_start_offset == 0) {
		Log2("Chunk starts at the beginnig of the file");
		return;
	}

	u64 start_from = _start_offset - how_much;

	char buffer[how_much];
	u32 n = 0;
	_file.read(how_much, start_from, buffer);

	// Making sure buffer starts at the beginning of the line. If buffer adjacent to end of previous
	// chunk, then we're ok. Otherwise, we have to find next available end of line and start from it.
	if (!eol_aligned) {
		while ((buffer[n] != '\n') && (n < how_much)) {
			n++;
		}
		if (buffer[n] == '\n') {
			if (n != how_much) {
				n++;
			}
		}
	}

	Log3("Chunk growing by " << how_much << " starting from " << start_from << " n=" << n);

	how_much -= n;

	if (how_much) {
		split_lines_reversed(buffer + n, how_much); 
	} else {
		Log1("Could not find proper end of line character in the buffer");
	}
	Log2("Chunk after growing: " << this);
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

std::ostream& operator<<(std::ostream& os, const Chunk& chunk)
{
	os << const_cast<Chunk&>(chunk);
	return os;
}

std::ostream& operator<<(std::ostream& os, const Chunk* chunk)
{
	os << const_cast<Chunk*>(chunk);
	return os;
}

