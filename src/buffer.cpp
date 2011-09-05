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

Buffer::iterator& Buffer::iterator::operator++(int) // suffix form
{
	_line_index++;
	if (_chunk->get_absolute_line_index(_line_index) >= _chunk->get_lines_count()) {
		_chunk->grow_down();
	}
	if (_chunk->get_absolute_line_index(_line_index) >= _chunk->get_lines_count()) {
		Log2("Iterator reached end of buffer...");
		*this = Buffer::end();
	}
	return *this;
}

Buffer::iterator Buffer::iterator::operator++() // prefix form
{
	int a = 0;
	Buffer::iterator it = *this;
	operator++(a);
	return it;
}

Buffer::iterator& Buffer::iterator::operator--(int) // suffix form
{
	if (_chunk->get_absolute_line_index(_line_index) == 0) {
		_chunk->grow_up();
	}

	if (_chunk->get_absolute_line_index(_line_index) == 0) {
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
	Log4("Allocated pointer " << std::hex << reinterpret_cast<void*>(line._string) << std::dec);
	_file.read(line._length, it->get_offset(), line._string);
	line._string[line._length] = 0;

	line._original_string = line._string;
	line._original_length = line._length;
}

Buffer::iterator Buffer::begin()
{
	if (_chunks.size() == 0) {
		Chunk* chunk = new Chunk(std::string(""), _file, 0, 0, 0, 0);
		chunk->grow_down();
		_chunks.push_back(chunk);
	}
	iterator it(_chunks[0], _chunks[0]->get_first_line_index());
	return it;
}

Buffer::iterator Buffer::back()
{
	// First, checking if last chunk in the chunk dequeu is reasonably close to the end of the buffer.
	Chunk* last_chunk = _chunks.back();
	if (_file.get_size() - last_chunk->get_end() < Config::chunk_grow_size) {
		last_chunk->grow_down();
	} else {
		last_chunk = new Chunk("100%", _file, _chunks.size(), _file.get_size(), 0, _chunks.back());
		last_chunk->grow_up();
		_chunks.push_back(last_chunk);
	}

	iterator it(last_chunk, last_chunk->get_last_line_index());
	Log2("Returning last line in the buffer " << it);
	return it;
}

std::ostream& operator<<(std::ostream& os, Buffer::iterator& it)
{
	os << "[Iterator: " << *it;
	if (it._chunk != 0) {
		os << " in " << it._chunk << ", index " << it._line_index;
	}
	os << "]";
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

