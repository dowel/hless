#ifndef __LINE_H__
#define __LINE_H__

#include <deque>
#include <string>
#include <list>
#include <boost/shared_ptr.hpp>

#include "log.h"
#include "types.h"

/**
 * Class below represents actual data in the file. Each instance contains single line of text in 
 * original file. Also, this class encapsulates basic operations on lines of text, such as splitting 
 * line into multiple lines, stripping the line and matching it against regular expressions. 
 */
class Line;
typedef std::list<boost::shared_ptr<Line> > LineList;

class Line
{
public:
	Line() : _length(0), _original_length(0), _string(0), _original_string(0), _temp_buffer(0) { }
	Line(char* str, u32 length);
	Line(Line& other);
	~Line();

	void init(char* ptr, u32 length);
	void init_copyless(char* ptr, u32 length);

	void reset();

	void strip_back();

	char* get_text()
	{
		return _string;
	}

	char* get_text(u32 max_length);
	void split_lines(u32 length, LineList& lst);

	friend class Buffer;
	friend std::ostream& operator<<(std::ostream& os, Line& line);

private:
	/**
	 * Note that the _length here does not include the terminating \0 character. Actually, the string is
	 * one byte longer than _length. 
	 */
	u32 _length;
	u32 _original_length;
	char* _string;
	char* _original_string;
	char* _temp_buffer;
};

std::ostream& operator<<(std::ostream& os, Line& line);
std::ostream& operator<<(std::ostream& os, Line* line);

#endif

