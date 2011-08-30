#include <cstring>

#include "log.h"
#include "buffer.h"
#include "config.h"

static __attribute__((unused)) const char* MODULE_NAME = "buffer";
Buffer::iterator Buffer::_last(0, (u64)-1);

Buffer::Buffer(Readable& file)
	: _file(file)
{

}

Buffer::~Buffer()
{

}

Buffer::Chunk::Chunk(std::string name, Readable& file, u32 chunk_index, u64 start)
	: _name(name)
	, _file(file)
	, _chunk_index(chunk_index)
	, _start(start)
	, _next(0)
	, _prev(0)
{ }

void Buffer::Chunk::split_lines(char* buffer, u32 length)
{
	u32 i = 0;
	u32 old_i = 0;
	while (i < length) {
		if (buffer[i] == '\n') {
			Buffer::MetaLine line(_start + _length + old_i, i - old_i + 1);
			Log2("Adding line " << line);
			_lines.push_back(line);
			old_i = i + 1;
		}
		i++;
	}
	_length += old_i;
}

void Buffer::Chunk::grow_up()
{
	Log1("Asked to grow chunk " << this << " up");
	Log2("Chunk before growing: " << this);
	if (_start == 0) {
		Log2("Chunk starts at the beginnig of the file");
		return;
	}

	if (_prev && _start == _prev->_start + _prev->_length) {
		Log2("Chunk beginning is adjacent to the end of previous chunk");
		return;
	}

	u64 start_from;
	u64 how_much;
	u64 rel_start;
	if (_prev) {
		rel_start = _prev->_start + _prev->_length;
	} else {
		rel_start = 0;
	}

	if (_start - rel_start <= Config::chunk_grow_size) {
		start_from = rel_start;
		how_much = _start - rel_start;
	} else {
		start_from = _start - Config::chunk_grow_size;
		how_much = Config::chunk_grow_size;
	}

	char buffer[how_much];
	u32 n = 0;
	_file.read(how_much, start_from, buffer);

	/**
	 * Making sure buffer starts at the beginning of the line. If buffer adjacent to 
	 * end of previous chunk, then we're ok. Otherwise, we have to find next available 
	 * end of line and start from it. 
	 */
	if (_prev && _prev->_start + _prev->_length != start_from) {
		while ((buffer[n] != '\n') && (n < how_much)) {
			n++;
		}
	}

	split_lines(&buffer[n], how_much - n);
	Log2("Chunk after growing: " << this);
}

void Buffer::Chunk::grow_down()
{
	Log1("Asked to grow chunk " << this << " down");
	Log2("Chunk before growing: " << this);

	if (_next && _next->_start == _start + _length) {
		Log2("Chunk end is adjacent to the beginning of next chunk");
		return;
	}

	u64 how_much = Config::chunk_grow_size;
	if (_next && (_next->_start - (_start + _length) < how_much)) {
		how_much = _next->_start - (_start + _length);
	}

	char buffer[how_much];
	u32 n =_file.read(Config::chunk_grow_size, _start + _length, buffer);
	split_lines(buffer, n);
	Log2("Chunk after growing: " << this);
}

Buffer::iterator& Buffer::iterator::operator ++(int) // suffix form
{
	_line_index++;
	if (_line_index >= _chunk->get_size()) {
		_chunk->grow_down();
	}
	if (_line_index >= _chunk->get_size()) {
		Log2("Iterator reached end of buffer...");
		*this = Buffer::end();
	}
	return *this;
}

Buffer::iterator Buffer::iterator::operator ++() // prefix form
{
	int a = 0;
	Buffer::iterator it = *this;
	operator++(a);
	return it;
}

Buffer::iterator& Buffer::iterator::operator--(int) // suffix form
{
	if (_line_index == 0) {
		u64 size = _chunk->get_size();
		_chunk->grow_up();
		_line_index = _chunk->get_size() - size;
	}

	if (_line_index == 0) {
		Log2("Iterator reached beginning of buffer...");
		*this = Buffer::end();
		return *this;
	}

	_line_index--;
	return *this;
}

Buffer::iterator Buffer::iterator::operator--() // prefix form
{
	int a = 0;
	Buffer::iterator it = *this;
	operator--(a);
	return it;
}

void Buffer::read_line(iterator& it, Line& line)
{
	line.reset();
	line._length = it->get_length();
	line._string = new char[line._length + 1];
	Log3("Allocated pointer " << std::hex << reinterpret_cast<void*>(line._string));
	_file.read(line._length, it->get_offset(), line._string);
	line._string[line._length] = 0;

	line._original_string = line._string;
	line._original_length = line._length;
}

Buffer::iterator Buffer::begin()
{
	if (_chunks.size() == 0) {
		Chunk* chunk = new Chunk(std::string(""), _file, 0, 0);
		chunk->grow_down();
		_chunks.push_back(chunk);
	}
	iterator it(_chunks[0], 0);
	return it;
}

std::ostream& operator<<(std::ostream& os, Buffer::iterator& it)
{
	os << "[Iterator: " << *it;
	if (it._chunk != 0) {
		os << " in " << it._chunk;
	}
	os << "]";
	return os;
}

std::ostream& operator<<(std::ostream& os, Buffer::Chunk& chunk)
{
	if (chunk._name.length() == 0) {
		os << "[Chunk: first chunk, ";
	} else {
		os << "[Chunk: " << chunk._name << ", ";
	}
	os << chunk._start << "-";
	os << chunk._start + chunk._length << "(+" << chunk._length << "), ";
	os << chunk._lines.size();
	os << "]";
	return os;
}

std::ostream& operator<<(std::ostream& os, Buffer::Chunk* chunk)
{
	os << *chunk;
	return os;
}

std::ostream& operator<<(std::ostream& os, Buffer::MetaLine& line)
{
	os << "[MetaLine: " << line.get_offset() << "-" << (line.get_offset() + line.get_length()) << "(+" << 
		line.get_length() << ")]";
	return os;
}

bool operator==(const Buffer::iterator& first, const Buffer::iterator& second)
{
	if ((first._chunk == second._chunk) && (first._line_index == second._line_index)) {
		return true;
	}

	return false;
}

bool operator!=(const Buffer::iterator& first, const Buffer::iterator& second)
{
	return !(first == second);
}

