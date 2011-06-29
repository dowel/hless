#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <deque>
#include <string>
#include <list>
#include <boost/shared_ptr.hpp>

#include "log.h"
#include "types.h"
#include "readable.h"
#include "error.h"
#include "line.h"

class Buffer
{
public:
	Buffer(Readable& readable);
	~Buffer();

	/**
	 * Class below encapsulates meta information about the line of text in the input file. Buffer class 
	 * caches these objects. In their essense they only represent lines, but cannot be used to retreive 
	 * the actual line. iterator class below does the connection between MetaLine (meta information) and
	 * Line (actual line) - both iterator and Line defined below.
	 */
	class MetaLine
	{
	public:
		MetaLine(u64 offset, u64 length) 
			: _offset(offset)
			, _length(length)
		{ }

		u64 get_offset() { return _offset; }
		u64 get_length() { return _length; }

	private:
		u64 _offset;
		u64 _length;
	};

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
		
		void split_lines(char* buffer, u32 length);
		void grow_up();
		void grow_down();

		friend std::ostream& operator<<(std::ostream& os, Buffer::Chunk& chunk);
		friend std::ostream& operator<<(std::ostream& os, Buffer::Chunk* chunk);

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

	/**
	 * This class puts together meta information about each line and it's location in the chunk and
	 * actual chunk.
	 */
	class iterator
	{
	public:
		iterator()
			: _chunk(0)
			, _line_index((u64)-1)
		{ }

		iterator(iterator& other)
			: _chunk(other._chunk)
			, _line_index(other._line_index)
		{ }

		iterator(Chunk* chunk, u64 line_index)
			: _chunk(chunk)
			, _line_index(line_index)
		{ }

		Chunk* get_chunk()
		{
			return _chunk;
		}

		u64 get_line_index()
		{
			return _line_index;
		}

		MetaLine& operator*()
		{
			if (_chunk == NULL) {
				throw Exception("Attempt to use uninitialized iterator");
			}
			return _chunk->get_line(_line_index);
		}

		MetaLine* operator->()
		{
			return &operator*();
		}

		iterator& operator++(int); // suffix form
		iterator operator++(); // prefix form

		iterator& operator--(int); // suffix form
		iterator operator--(); // prefix form

		friend std::ostream& operator<<(std::ostream& os, Buffer::iterator& it);
		friend bool operator==(const Buffer::iterator& first, const Buffer::iterator& second);
		friend bool operator!=(const Buffer::iterator& first, const Buffer::iterator& second);

	private:
		Chunk* _chunk;
		u64 _line_index;
	};

	typedef std::deque<Chunk*> ChunkList;

	void read_line(iterator& it, Line& line);

	iterator begin();
	static iterator& end()
	{
		return _last;
	}

private:
	Readable& _file;
	ChunkList _chunks;

	static iterator _last;
};

std::ostream& operator<<(std::ostream& os, Buffer::iterator& it);
std::ostream& operator<<(std::ostream& os, Buffer::Chunk& chunk);
std::ostream& operator<<(std::ostream& os, Buffer::Chunk* chunk);
std::ostream& operator<<(std::ostream& os, Buffer::MetaLine& line);
bool operator==(const Buffer::iterator& first, const Buffer::iterator& second);
bool operator!=(const Buffer::iterator& first, const Buffer::iterator& second);

#endif

