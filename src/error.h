#ifndef __ERROR_H__
#define __ERROR_H__

#include <exception>
#include <iostream>
#include <sstream>

#include <stdlib.h>
#include <execinfo.h>

class Backtrace
{
public:
	Backtrace() 
	{
		_n = backtrace(_ptrs, 38);
	}

	~Backtrace()
	{
		_n = 0;
	}

	friend std::ostream& operator<<(std::ostream& os, const Backtrace& bt);

private:
	void* _ptrs[38];
	int _n;
};

class Exception : public std::exception
{
public:
	Exception()
	{ 
		_ss << "General error" << std::endl; 
		_ss << "At: " << Backtrace() << std::endl;
	}
	Exception(std::string error)
	{
		_ss << error.c_str() << std::endl;
		_ss << "At: " << Backtrace() << std::endl;
	}

	Exception(const Exception& other)
	{
		_ss << other._ss.str().c_str();
	}

    virtual ~Exception() throw() { }

	virtual const char* what() const throw()
	{
		return _ss.str().c_str();
	}

private:
	std::stringstream _ss;
};

#endif

