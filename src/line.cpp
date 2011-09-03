#include "log.h"
#include "line.h"

#include <cstring>

static __attribute__((unused)) const char* MODULE_NAME = "line";

Line::Line(char* str, u32 length)
	: _temp_buffer(0)
{
	init(str, length);
}

Line::Line(Line& other)
	: _temp_buffer(0)
{
	init(other._string, other._length);
}

Line::~Line()
{
	reset();
}

void Line::init(char* str, u32 length)
{
	_length = length;
	_original_length = length;
	_string = new char[_length + 1];
	Log3("Allocated pointer " << std::hex << reinterpret_cast<void*>(_string) << std::dec);
	_original_string = _string;
	memcpy(_string, str , _length);
	_string[_length] = 0;
}

void Line::init_copyless(char* str, u32 length)
{
	_length = length;
	_original_length = length;
	_string = str;
	_original_string = str;
}

void Line::reset()
{
	if (_original_string) {
		Log3("Deleting pointer " << std::hex << reinterpret_cast<void*>(_original_string) << std::dec);
		delete _original_string;
	}

	if (_temp_buffer) {
		Log3("Deleting pointer " << std::hex << reinterpret_cast<void*>(_temp_buffer) << std::dec);
		delete _temp_buffer;
	}
}

void Line::strip_back()
{
	Log2("Line before stripping: " << this);
	if (_length == 0) {
		return;
	}
	while (!isprint(_string[_length - 1])) {
		_length--;
	}
	_string[_length] = 0;
	Log2("Line after stripping: " << this);
}

char* Line::get_text(u32 max_length)
{
	if (_temp_buffer) {
		Log3("Deleting pointer " << std::hex << reinterpret_cast<void*>(_temp_buffer) << std::dec);
		delete _temp_buffer;
	}

	_temp_buffer = new char[max_length + 1];
	Log3("Allocated pointer " << std::hex << reinterpret_cast<void*>(_temp_buffer) << std::dec);
	u32 len = max_length > _length ? _length : max_length;
	memcpy(_temp_buffer, _string, len);
	_temp_buffer[max_length] = 0;
	return _temp_buffer;
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
			memcpy(buffer, _string + cnt, _length - cnt);
			memset(buffer + _length - cnt, ' ', length - (_length - cnt));
			temp = buffer;
		} else {
			temp = get_text() + cnt;
		}
		lst.push_back(boost::shared_ptr<Line>(new Line(temp, length)));
		cnt += length;
	}
}

std::ostream& operator<<(std::ostream& os, Line& line)
{
	const u32 buffer_length = 64;
	char buffer[buffer_length + 1];
	if (line._string != 0) {
		u32 len = line._length > buffer_length ? buffer_length : line._length;
		memcpy(buffer, line._string, len);
		buffer[len] = 0;
	}
	os << "[Line: " << buffer << "... (" << line._length << " bytes)]";
	return os;
}

std::ostream& operator<<(std::ostream& os, Line* line)
{
	return os << *line;
}

