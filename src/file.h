#ifndef __FILE_H__
#define __FILE_H__

#include <string>

#include "readable.h"

class File : public Readable
{
public:
	File() { };
	~File() { };

	virtual void open(std::string filename) = 0;
	virtual void close() = 0;

private:
	std::string _filename;
};

#endif

