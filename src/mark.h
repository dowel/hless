#ifndef __MARK_H__
#define __MARK_H__

#include <set>

#include "buffer.h"
#include "brush.h"

class Mark
{
public:
	Mark(const Buffer::iterator& it) : _it(it), _color(0) { }

	void next_color() const
	{
		_color = (_color + 1) % Brush::marks.size();
	}

	Color* color() const { return Brush::marks[_color]; }
	int get_color_index() const { return _color; }
	Buffer::iterator& get_iterator() { return _it; }

	bool operator==(const Mark& other)
	{
		if (other._color == _color && other._it == _it) {
			return true;
		} else {
			return false;
		}
	}

	friend bool operator<(const Mark& first, const Mark& second);
	friend std::ostream& operator<<(std::ostream& os, Mark& mark);

private:
	Buffer::iterator _it;
	mutable int _color;
};

typedef std::set<Mark> Marks;


#endif

