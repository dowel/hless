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
#include "meta_line.h"
#include "chunk.h"

class Buffer
{
public:
	Buffer(Readable& readable);
	~Buffer();

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

	/** 
	 * Returns iterator to the first line in the buffer. 
	 * 
	 * @return iterator 
	 */
	iterator begin();

	/**
	 * Returns iterator to the end of the buffer. Note that there's no line behind it. It's purpose is 
	 * to mark that we've reached end of the buffer. 
	 * 
	 * @return iterator& 
	 */
	static iterator& end()
	{
		return _last;
	}

	/**
	 * Returns iterator pointing to the last line in the buffer. Unline end(), this routine returns 
	 * iterator to actual last line. 
	 * 
	 * @return iterator 
	 */
	iterator back();

private:
	Readable& _file;
	ChunkList _chunks;

	static iterator _last;
};

std::ostream& operator<<(std::ostream& os, Buffer::iterator& it);
bool operator==(const Buffer::iterator& first, const Buffer::iterator& second);
bool operator!=(const Buffer::iterator& first, const Buffer::iterator& second);

#endif

