#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "plain_file.h"
#include "error.h"
#include "log.h"

static const __attribute__((unused)) char* MODULE_NAME = "plain";

PlainFile::PlainFile()
	: _fd(-1)
{

}

PlainFile::~PlainFile()
{

}

void PlainFile::open(std::string filename)
{
	_fd = ::open(filename.c_str(), O_RDONLY);
	if (_fd < 0) {
		throw Exception("Error opening file");
	}
}

void PlainFile::close()
{
	if (_fd > 0) {
		::close(_fd);
	}
}

u32 PlainFile::read(u32 how_much, u64 offset, char* buffer)
{
	s32 n = ::pread(_fd, buffer, how_much, offset);
	if (n <= 0) {
		std::ostringstream os;
		os << "pread() returned " << n << ". errno set to " << errno;
		throw Exception(os.str());
	}

	return n;
}

u64 PlainFile::get_size()
{
	struct stat st;
	if (fstat(_fd, &st) < 0) {
		std::ostringstream os;
		os << "Failed to obtain file size " << errno;
		throw Exception(os.str());
	}

	return st.st_size;
}

