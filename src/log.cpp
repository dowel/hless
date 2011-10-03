#include <boost/date_time/posix_time/posix_time.hpp>
#include <time.h>
#include <fstream>

#include "log.h"

LogStream* Logging::_stream = 0;
Logging::MinLevelsMap Logging::_min_levels;

const int Logging::LOG_LEVEL_ERROR = 0;
const int Logging::LOG_LEVEL_WARN = 1;

const int Logging::LOG_LEVEL1 = 10;
const int Logging::LOG_LEVEL2 = 11;
const int Logging::LOG_LEVEL3 = 12;
const int Logging::LOG_LEVEL4 = 13;
const int Logging::LOG_LEVEL5 = 15;

const int Logging::DEFAULT_LOG_LEVEL = 10;

void Logging::open(std::string logfile)
{
	if (_stream == 0) {
		_stream = new LogStream(logfile);
	}
}

void Logging::close()
{
	delete _stream;
}

LogStream* Logging::instance()
{
	return _stream;
}

void Logging::SetMinLevel(const char* module, int level)
{
	_min_levels[module] = LogLevel(level);
}

LogLevel Logging::GetMinLevel(const char* module)
{
	return _min_levels[module];
}

LogStream::LogStream(std::string logfile)
{
	_os.open(logfile.c_str(), std::fstream::out | std::fstream::trunc);
}

LogStream::~LogStream()
{
	_os.close();
}

std::ostream& LogStream::WriteHeaders(const char* file, int line, const char* func, 
	const char* module, int level)
{
	_os << boost::posix_time::from_time_t(time(0)) << " [" << std::setw(10) << std::right << 
		module << std::setw(0) << ":" << std::setw(4) << std::left << line << "]" << std::setw(0);
	switch (level) {
	case Logging::LOG_LEVEL_ERROR:  _os << "[--- ERROR ---]"; break;
	case Logging::LOG_LEVEL_WARN:   _os << "[WARN]"; break;
	case Logging::LOG_LEVEL1:   	_os << "[DBG1]"; break;
	case Logging::LOG_LEVEL2:   	_os << "[DBG2]"; break;
	case Logging::LOG_LEVEL3:   	_os << "[DBG3]"; break;
	case Logging::LOG_LEVEL4:   	_os << "[DBG4]"; break;
	case Logging::LOG_LEVEL5:   	_os << "[DBG5]"; break;
	}
	_os << " ";
	return _os;
}

