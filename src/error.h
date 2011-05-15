#ifndef __ERROR_H__
#define __ERROR_H__

#include <exception>
#include <iostream>
#include <sstream>

class Exception : public std::exception
{
public:
	Exception() : _error("General error") { }
	Exception(std::string error) : _error(error) { }

    virtual ~Exception() throw() { }

	virtual const char* what() const throw()
	{
        return _error.c_str();
	}

private:
    std::string _error;
};

#endif

