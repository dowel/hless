#include <cstring>

#include "log.h"
#include "buffer.h"
#include "config.h"

static __attribute__((unused)) const char* MODULE_NAME = "buffer";
Buffer::iterator Buffer::_last(0, (u64)-1, 0);

Buffer::Buffer(Readable& file)
	: _file(file)
{

}

Buffer::~Buffer()
{

}

Buffer::iterator& Buffer::iterator::operator++(int) // suffix form
{
	Log3("Before incrementing " << *this);
	_line_index++;
	if (_chunk->get_absolute_line_index(_line_index) >= _chunk->get_lines_count()) {
		// Checking if there's by a small chance next chunk and we're bumping into it's boundary.
		// If so, we want to move to next chunk...
		Chunk* next = _buffer->get_next_chunk(_chunk);
		if (next) {
			Log3("Current chunk end at " << _chunk->get_end() << ". Next chunk start " << next->get_start_offset());
		}

		if (next && _chunk->get_end() == next->get_start_offset()) {
			_line_index = next->get_first_line_index();
			_chunk = next;
		} else {
			_buffer->grow_chunk_down(_chunk);
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
		Log3("Iterator at beginning of chunk: " << *this);

		// Checking if there's by a small chance previous chunk and we're bumbing into it's boundary.
		// If so, we want to move to previous chunk...
		Chunk* prev = _buffer->get_prev_chunk(_chunk);
		if (prev) {
			Log3("Current chunk start at " << _chunk->get_start_offset() << ". Next chunk start " << prev->get_end());
		}

		if ((prev != 0) && (prev->get_end() == _chunk->get_start_offset())) {
			_line_index = prev->get_last_line_index();
			_chunk = prev;
			return *this;
		} else {
			_chunk = _buffer->grow_chunk_up(_chunk);
		}
	}

	if (_chunk->get_absolute_line_index(_line_index) == 0) {
		Log2("Iterator reached beginning of buffer...");
		*this = Buffer::end();
		return *this;
	}

	Log3("Before decrementing " << *this);
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
		Chunk* next = _buffer->get_next_chunk(_chunk);
		if (next && _chunk->get_end() == next->get_start_offset()) {
			count -= _chunk->get_lines_count() - _chunk->get_absolute_line_index(_line_index);
			_line_index = next->get_first_line_index();
			_chunk = next;
			Log1("Skipping to chunk " << _chunk << ", line index " << _line_index << ", count " << count);
		} else {
			u32 length = _chunk->get_lines_count();
			_buffer->grow_chunk_down(_chunk);
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
	Chunk* chunk;
	if (_chunks.size() == 0) {
		chunk = new Chunk(std::string(""), _file, 0);
		_chunks[chunk->get_start_offset()] = chunk;
		grow_chunk_down(chunk);
	} else {
		chunk = _chunks.begin()->second;
	}
	
	iterator it(chunk, chunk->get_first_line_index(), this);
	return it;
}

Buffer::iterator Buffer::back()
{
	bool add_chunk = false;
	Chunk* chunk = 0;
	if (_chunks.size() == 0) {
		add_chunk = true;
	} else {
		chunk = _chunks.rbegin()->second;
		Log3("Last chunk is " << chunk << ". File's end " << _file.get_size());
		if (_file.get_size() - chunk->get_end() >= Config::chunk_grow_size) {
			add_chunk = true;
		}
	}

	if (!add_chunk) {
		chunk->grow_down();
	} else {
		chunk = new Chunk("100%", _file, _file.get_size());
		_chunks[chunk->get_start_offset()] = chunk;
		grow_chunk_up(chunk);
	}

	iterator new_it(chunk, chunk->get_last_line_index(), this);
	Log2("Returning last line in the buffer " << new_it);
	return new_it;
}

Buffer::iterator Buffer::offset(u64 offset)
{
	Log1("Asked to return iterator at offset " << offset);

	Chunk* chunk;
	s64 index = 0;

	// First we check if there is chunk that contains specified offset.
	auto it = std::find_if(_chunks.begin(), _chunks.end(), 
		[&offset](const std::pair<u64, Chunk*>& p) -> bool 
	{ 
		return (p.first <= offset) && (offset < p.second->get_end()); 
	});

	if (it != _chunks.end()) {
		index = it->second->get_index_at_offset(offset);
		Log1("Returning chunk " << it->second << ", index " << index);
		return iterator(it->second, index, this);
	} else {
		Log2("Did not find chunk. Trying chunk that can grow.");
	}

	// Now check if we have a chunk that we can grow either up or down.
	it = std::find_if(_chunks.begin(), _chunks.end(), 
		[&offset](const std::pair<u64, Chunk*>& p) -> bool 
	{ 
		u64 inf, sup;
		if (p.first > Config::chunk_grow_size) {
			inf = p.first - Config::chunk_grow_size;
		} else {
			inf = 0;
		}

		sup = offset + Config::chunk_grow_size;

		return (inf <= offset) && (offset < sup);
	});

	if (it != _chunks.end()) {
		if (offset < it->first) {
			Log1("Growing chunk up");
			grow_chunk_up(it->second);
		} else {
			Log1("Growing chunk down");
			grow_chunk_down(it->second);
		}

		index = it->second->get_index_at_offset(offset);
		Log1("Returning chunk " << *it->second << ", index " << index);
		return iterator(it->second, index, this);
	} else {
		Log2("Did not find chunk that can grow. Creating new chunk.");
	}

	// Create new chunk.
	chunk = new Chunk(STR("@" << offset), _file, offset - (Config::chunk_grow_size / 2));
	_chunks[chunk->get_start_offset()] = chunk;
	grow_chunk_down(chunk);

	Log1("Created new chunk " << chunk);

	index = it->second->get_index_at_offset(offset);
	Log1("Returning chunk " << *it->second << ", index " << index);
	return iterator(it->second, index, this);
}

s64 Buffer::iterator::distance_lines(const Buffer::iterator& other)
{
	const iterator* a;
	const iterator* b;
	int modifier = 1;
	s64 counter = 0;
	Chunk* ca;
	Chunk* cb;

	// The distance can be from this to other or from other to this. Eventually we need
	// A at the beginning and B at the end.
	if (_chunk->get_start_offset() == other._chunk->get_start_offset()) {
		return _line_index - other._line_index;
	} else {
		if (_chunk->get_start_offset() > other._chunk->get_start_offset()) {
			a = this;
			b = &other;
		} else {
			a = &other;
			b = this;
			modifier = -1;
		}
	}

	ca = a->_chunk;
	cb = b->_chunk;
	counter += ca->get_lines_count() - ca->get_absolute_line_index(a->_line_index);

	ca = _buffer->get_next_chunk(ca);
	if (!ca || !a->_chunk->adjacent_chunks(ca)) {
		return -1;
	}

	while (ca->get_start_offset() < cb->get_start_offset()) {
		Chunk* temp = _buffer->get_next_chunk(ca);
		if (!temp || !ca->adjacent_chunks(temp)) {
			return -1;
		}
		counter += ca->get_lines_count();
		ca = temp;
	}

	counter += cb->get_absolute_line_index(b->_line_index);
	counter *= modifier;

	return counter;
}

Chunk* Buffer::grow_chunk_down(Chunk* chunk)
{
	if (chunk == 0) {
		return 0;
	}

	Chunk* next = get_next_chunk(chunk);
	if (next == 0 || (next->get_start_offset() - chunk->get_end() > Config::chunk_grow_size)) {
		chunk->grow_down();
		return chunk;
	}

	chunk->grow_down(std::min(Config::chunk_grow_size, next->get_start_offset() - chunk->get_end()));

	return chunk; 
}

Chunk* Buffer::grow_chunk_up(Chunk* chunk)
{
	if (chunk == 0) {
		return 0;
	}

	Chunk* prev = get_prev_chunk(chunk);
	if (prev == 0 || (chunk->get_start_offset() - prev->get_end() > Config::chunk_grow_size)) {
		_chunks.erase(chunk->get_start_offset());
		chunk->grow_up(false);
		_chunks[chunk->get_start_offset()] = chunk;
		return chunk;
	}

	_chunks.erase(chunk->get_start_offset());
	chunk->grow_up(std::min(Config::chunk_grow_size, chunk->get_start_offset() - prev->get_end()), true);
	_chunks[chunk->get_start_offset()] = chunk;

	return chunk;
}

Chunk* Buffer::get_prev_chunk(const Chunk* chunk)
{
	if (chunk == 0) {
		return 0;
	}

	Buffer::ChunkList::iterator it = _chunks.find(chunk->get_start_offset());
	if (it == _chunks.end()) {
		if (_chunks.size() != 1) {
			Error("Could not find chunk " << chunk << " in chunks data structure(" << 
				_chunks.size() << ") from " << Backtrace()); 
		}
		return 0;
	}

	it--;
	if (it == _chunks.end()) {
		return 0;
	}

	return it->second;
}

Chunk* Buffer::get_next_chunk(const Chunk* chunk)
{
	if (chunk == 0) {
		return 0;
	}

	Buffer::ChunkList::iterator it = _chunks.find(chunk->get_start_offset());
	if (it == _chunks.end()) {
		if (_chunks.size() != 1) {
			Error("Could not find chunk " << chunk << " in chunks data structure (" << 
				_chunks.size() << ") from " << Backtrace()); 
		}
		return 0;
	}

	it++;
	if (it == _chunks.end()) {
		return 0;
	}

	return it->second;
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

bool operator<(const Buffer::iterator& first, const Buffer::iterator& second)
{
	if (first._chunk->get_start_offset() != second._chunk->get_start_offset()) {
		return first._chunk->get_start_offset() < second._chunk->get_start_offset();
	}

	return first._line_index < second._line_index;
}

