#ifndef __META_LINE_H__
#define __META_LINE_H__

#include <ostream>

#include "types.h"

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

	u64 get_offset() const { return _offset; }
	u64 get_length() const { return _length; }
	u64 get_end() const { return _offset + _length; }

private:
	u64 _offset;
	u64 _length;
};

std::ostream& operator<<(std::ostream& os, MetaLine& line);

#endif

