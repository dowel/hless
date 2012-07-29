#ifndef __LINE_H__
#define __LINE_H__

#include <deque>
#include <string>
#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

#include "log.h"
#include "types.h"

/**
 * Class below represents actual data in the file. Each instance contains single line of text in 
 * original file. Also, this class encapsulates basic operations on lines of text, such as splitting 
 * line into multiple lines, stripping the line and matching it against regular expressions. 
 *  
 * It could have been lovely if we could inherit from std::string, but we can't.  
 */
class Line;

/**
 * LineList has special purpose. Buffer contains lines of text. Not every line fit onto screen. So, 
 * we have to split buffer lines into screen lines. LineList represents list of screen lines. 
 */
typedef std::list<boost::shared_ptr<Line> > LineList;

class Line
{
public:
	Line() : _length(0) { }
	Line(char* str);
	Line(const char* str);
	Line(char* str, u32 length);
	Line(std::string str);
	Line(const Line& other);
	Line(char c, u32 how_many);
	~Line();

	void init(const char* ptr, u32 length);
	void init_copyless(char* ptr, u32 length);

	void strip_back();

	const char* get()
	{
		return _string.get();
	}

	const char* get(u32 max_length);
	u32 length() { return _length; }

	void split_lines(u32 length, LineList& lst);

	Line substr(u32 start, u32 length);

	char& operator[](const u32& index);
	Line& operator+=(const Line& other);

	friend class Buffer;
	friend std::ostream& operator<<(std::ostream& os, Line& line);

private:
	u32 _length;
	boost::shared_array<char> _string;
	boost::shared_array<char> _temp_buffer;
	Line* _temp_line; // used to return substrings...

	static const u32 min_buffer_to_allocate = 256;
};

std::ostream& operator<<(std::ostream& os, Line& line);
std::ostream& operator<<(std::ostream& os, Line* line);

#endif

