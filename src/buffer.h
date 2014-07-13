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
		iterator(Buffer* buffer)
			: _chunk(0)
			, _line_index((u64)-1)
			, _buffer(buffer)
		{ }

		iterator(const iterator& other)
			: _chunk(other._chunk)
			, _line_index(other._line_index)
			, _buffer(other._buffer)
		{ }

		iterator(Chunk* chunk, u64 line_index, Buffer* buffer)
			: _chunk(chunk)
			, _line_index(line_index)
			, _buffer(buffer)
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

		std::string string_name()
		{
			std::ostringstream ss;
			std::string chunk_name = _chunk->get_name();
			if (chunk_name != "") {
				ss << chunk_name;
			}

			if ((chunk_name != "") && (_line_index > 0)) {
				ss << "+";
			}

			ss << _line_index;

			return std::string(ss.str());
		}

		iterator& operator++(int); // suffix form
		iterator operator++(); // prefix form

		iterator& operator--(int); // suffix form
		iterator operator--(); // prefix form

		iterator& operator+=(int);
		iterator& operator-=(int);

		friend std::ostream& operator<<(std::ostream& os, Buffer::iterator& it);
		friend bool operator==(const Buffer::iterator& first, const Buffer::iterator& second);
		friend bool operator!=(const Buffer::iterator& first, const Buffer::iterator& second);

	private:
		Chunk* _chunk;
		s64 _line_index;
		Buffer* _buffer;
	};

	typedef std::map<u64, Chunk*> ChunkList;

	/**
	 * Reads specified by iterator line into Line object.
	 * 
	 * @param it 
	 * @param line 
	 */
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

	/**
	 * Returns iterator the points to specified offset in the file.
	 *  
	 * @param offset 
	 * 
	 * @return iterator 
	 */
	iterator offset(u64 offset);

	/** 
	 * Grows specified chunk down. If necessary, merges it with next chunk. 
	 *  
	 * @param chunk 
	 *  
	 * @return Chunk* returns pointer to chunk. 
	 */
	Chunk* grow_chunk_down(Chunk* chunk);

	/**
	 * Grows chunk up. If necessary merges it with previous chunk. Returns chunk that contains whatever 
	 * input chunk contained. 
	 * 
	 * @param chunk 
	 * 
	 * @return Chunk* 
	 */
	Chunk* grow_chunk_up(Chunk* chunk);

	/**
	 * Find chunk that follow argument chunk.
	 * 
	 * @param chunk 
	 * 
	 * @return Chunk* 
	 */
	Chunk* get_next_chunk(const Chunk* chunk);

	/**
	 * Finds chunk that comes before specified chunk.
	 * 
	 * @param chunk 
	 * 
	 * @return Chunk* 
	 */
	Chunk* get_prev_chunk(const Chunk* chunk);

private:
	Readable& _file;
	ChunkList _chunks;

	static iterator _last;
};

std::ostream& operator<<(std::ostream& os, Buffer::iterator& it);
bool operator==(const Buffer::iterator& first, const Buffer::iterator& second);
bool operator!=(const Buffer::iterator& first, const Buffer::iterator& second);

#endif

