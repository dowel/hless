#include "log.h"
#include "line.h"
#include "error.h"

#include <cstring>

static __attribute__((unused)) const char* MODULE_NAME = "line";

Line::Line(char* str)
{
	init(str, strlen(str));
}

Line::Line(const char* str)
{
	init(str, strlen(str));
}

Line::Line(char* str, u32 length)
{
	init(str, length);
}

Line::Line(std::string str)
{
	init(str.c_str(), str.length());
}

Line::Line(const Line& other)
{
	init(other._string.get(), other._length);
}

Line::Line(char c, u32 how_many)
{
	char* s = new char[how_many + 1];
	memset(s, c, how_many);
	s[how_many] = 0;
	init_copyless(s, how_many);
}

Line::~Line()
{
}

void Line::init(const char* str, u32 length)
{
	_length = length;
	_string.reset(new char[_length + 1]);
	memcpy(_string.get(), str , _length + 1);
	_string[_length] = 0;
}

void Line::init_copyless(char* str, u32 length)
{
	// This is a little ugly trick. We didn't allocate the memory here, but since we're given this
	// memory buffer and this is the only module that tracks memory, we print a notification from here.
	_length = length;
	_string.reset(str);
}

void Line::strip_back()
{
	Log2("Line before stripping: " << this);
	if (_length == 0) {
		return;
	}
	while (!isprint(_string[_length])) {
		_length--;
	}
	_string[_length] = 0;
	Log2("Line after stripping: " << this);
}

const char* Line::get(u32 max_length)
{
	_temp_buffer.reset(new char[max_length + 1]);
	u32 len = max_length > _length ? _length : max_length;
	memcpy(_temp_buffer.get(), _string.get(), len);
	_temp_buffer[max_length] = 0;
	return _temp_buffer.get();
}

void Line::split_lines(u32 length, LineList& lst)
{
	u32 cnt = 0;
	char buffer[length + 1];
	char* temp;
	Log2("Splitting " << _length << " byte line to " << length << " bytes long chunks.");
	while (cnt < _length) {
		Log2("Processed " << cnt << " so far");
		if (_length - cnt < length) { // string's tail...
			buffer[length] = 0;
			Log2("Copying " << _length - cnt << " bytes into temporary buffer");
			Log2("Writing spaces from " << _length - cnt << " for " << length - (_length - cnt) << " bytes");
			memcpy(buffer, _string.get() + cnt, _length - cnt);
			memset(buffer + _length - cnt, ' ', length - (_length - cnt));
			temp = buffer;
		} else {
			temp = _string.get() + cnt;
		}
		lst.push_back(boost::shared_ptr<Line>(new Line(temp, length)));
		cnt += length;
	}
}

char& Line::operator[](const u32& index)
{
	if (index > _length) {
		throw Exception(STR("Attempt to access beyond end of line. Line '" << get(64) << "', index " << index));
	}
	return _string[index];
}

Line& Line::operator+=(const Line& b)
{
	boost::shared_array<char> temp = _string;
	_string.reset(new char[_length + b._length + 1]);
	memcpy(_string.get(), temp.get(), _length);
	memcpy(_string.get() + _length, b._string.get(), b._length);
	_length = _length + b._length;
	_string[_length] = 0;
	return *this;
}

Line Line::substr(u32 start, u32 length)
{
	return Line(_string.get() + start, length);
}

std::ostream& operator<<(std::ostream& os, Line& line)
{
	const u32 buffer_length = 64;
	char buffer[buffer_length + 1];
	if (line._string.get() != 0) {
		u32 len = line._length > buffer_length ? buffer_length : line._length;
		memcpy(buffer, line._string.get(), len);
		buffer[len] = 0;
	}
	os << "[Line: " << buffer << "... (" << line._length << " bytes)]";
	return os;
}

std::ostream& operator<<(std::ostream& os, Line* line)
{
	return os << *line;
}

