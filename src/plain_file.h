#ifndef __PLAIN_FILE_H__
#define __PLAIN_FILE_H__

#include "file.h"
#include "types.h"

class PlainFile : public File
{
public:
	PlainFile();
	virtual ~PlainFile();

	virtual void open(std::string filename);
	virtual void close();
	virtual u32 read(u32 how_much, u64 offset, char* buffer);
	virtual u64 get_size();

private:
	int _fd;
};

#endif
