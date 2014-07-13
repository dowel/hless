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
		// Checking if there's by a small chance next chunk and we're bumping into it's boundary.
		// If so, we want to move to next chunk...
		Chunk* next = _buffer.get_next_chunk(_chunk);
		if (next && _chunk->get_end() == next->get_start_offset()) {
			_line_index = next->get_first_line_index();
			_chunk = next;
		} else {
			_buffer.grow_chunk_down(_chunk);
		}
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
		// Checking if there's by a small chance previous chunk and we're bumbing into it's boundary.
		// If so, we want to move to previous chunk...
		Chunk* prev = _buffer.get_prev_chunk(_chunk);
		if ((prev != 0) && (prev->get_end() == _chunk->get_start_offset())) {
			_line_index = prev->get_last_line_index();
			_chunk = prev;
			return *this;
		} else {
			_chunk = _buffer.grow_chunk_up(_chunk);
		}
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

Buffer::iterator& Buffer::iterator::operator+=(int count)
{
	Log1("Asked to add " << count << " to iterator " << *this);
	while (_chunk->get_absolute_line_index(_line_index + count) >= _chunk->get_lines_count()) {
		// Checking if there's by a small chance next chunk and we're bumbing into it's boundary.
		// If so, we want to move to next chunk...
		Chunk* next = _chunk->get_next();
		if (next && _chunk->get_end() == next->get_start_offset()) {
			count -= _chunk->get_lines_count() - _chunk->get_absolute_line_index(_line_index);
			_line_index = next->get_first_line_index();
			_chunk = next;
			Log1("Skipping to chunk " << _chunk << ", line index " << _line_index << ", count " << count);
		} else {
			u32 length = _chunk->get_lines_count();
			_chunk->grow_down();
			Log1("Current chunk has to next. Grew it to " << _chunk);
			if (length == _chunk->get_lines_count()) { 
				// This means chunk can no longer grow, i.e. reached end of buffer.
				break;
			}
		}
	}

	_line_index += count;
	return *this;
}

Buffer::iterator& Buffer::iterator::operator-=(int count) 
{
	return *this;
}

void Buffer::read_line(iterator& it, Line& line)
{
	u32 size = it->get_length() + 1;
	char* temp = new char[size];
	_file.read(size, it->get_offset(), temp);
	line.init_copyless(temp, it->get_length());
	line[line.length()] = 0;
}

Buffer::iterator Buffer::begin()
{
	if (_chunks.size() == 0) {
		Chunk* chunk = new Chunk(std::string(""), _file, 0, 0, 0, 0);
		grow_chunk_down(chunk);
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
		grow_chunk_up(last_chunk);
		_chunks.push_back(last_chunk);
	}

	iterator it(last_chunk, last_chunk->get_last_line_index());
	Log2("Returning last line in the buffer " << it);
	return it;
}

Buffer::iterator Buffer::offset(u64 offset)
{
}

Chunk* Buffer::grow_chunk_down(Chunk* chunk)
{
	if (chunk == 0) {
		return 0;
	}

	Chunk* next = get_next_chunk(chunk);
	if (next == 0) {
		chunk->grow_down();
		return chunk;
	}

	if (chunk->get_end() + Config::chunk_grow_size > next->get_start_offset()) {
		chunk->grow_down(next->get_start_offset() - chunk->get_end());
		chunk->append_other(next);
		delete_chunk(next);
		return chunk;
	}

	return 0; 
}

Chunk* Buffer::grow_chunk_up(Chunk* chunk)
{
	if (chunk == 0) {
		return 0;
	}

	Chunk* prev = get_prev_chunk(chunk);
	if (prev == 0) {
		chunk->grow_up();
		return chunk;
	}

	if (prev->get_end() + Config::chunk_grow_size > chunk->get_start_offset()) {
		prev->grow_down(prev->get_start_offset() - chunk->get_start_offset());
		prev->append_other(chunk);
		delete_chunk(chunk);
		return prev;
	}

	return 0;
}

Chunk* Buffer::get_prev_chunk(Chunk* chunk)
{
	if (chunk == 0) {
		return 0;
	}

	Buffer::ChunkList::iterator it = _chunks.find(chunk->get_start_offset());
	if (it == _chunks.end()) {
		return 0;
	}

	it--;
	if (it == _chunks.end()) {
		return 0;
	}

	return it->second;
}

Chunk* Buffer::get_next_chunk(Chunk* chunk)
{
	if (chunk == 0) {
		return 0;
	}

	Buffer::ChunkList::iterator it = _chunks.find(chunk->get_start_offset());
	if (it == _chunks.end()) {
		return 0;
	}

	it++;
	if (it == _chunks.end()) {
		return 0;
	}

	return it->second;
}

void Buffer::delete_chunk(Chunk* chunk)
{
	_chunks.erase(chunk->get_start_offset());
	delete chunk;
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

