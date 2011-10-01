#ifndef __LOG_H__
#define __LOG_H__

#include <string>
#include <fstream>
#include <sstream>
#include <map>

#define STR(s) (StringStream() << s).str()

#define Log(level, s) \
	if (LogLevel(level) >= Logging::GetMinLevel(MODULE_NAME)) { \
		Logging::instance()->WriteHeaders(__FILE__, __LINE__, __FUNCTION__, MODULE_NAME, level) << s << std::endl; \
	}

#define Log1(s) Log(Logging::LOG_LEVEL1, s)
#define Log2(s) Log(Logging::LOG_LEVEL2, s)
#define Log3(s) Log(Logging::LOG_LEVEL3, s)
#define Log4(s) Log(Logging::LOG_LEVEL4, s)
#define Log5(s) Log(Logging::LOG_LEVEL5, s)

#define Error(s) Log(Logging::LOG_LEVEL_ERROR, s)
#define Warn(s) Log(Logging::LOG_LEVEL_ERROR, s)

#define ContLog(level, s) \
	if (LogLevel(level) >= Logging::GetMinLevel(MODULE_NAME)) { \
		*Log::instance() << s; \
	}

#define LogCont1(s) ContLog(Logging::LOG_LEVEL1, s)
#define LogCont2(s) ContLog(Logging::LOG_LEVEL2, s)
#define LogCont3(s) ContLog(Logging::LOG_LEVEL3, s)
#define LogCont4(s) ContLog(Logging::LOG_LEVEL4, s)
#define LogCont5(s) ContLog(Logging::LOG_LEVEL5, s)

#define ContError(s) ContLog(Logging::LOG_LEVEL_ERROR, s)
#define ContWarn(s) ContLog(Logging::LOG_LEVEL_ERROR, s)

#define LogLevelEnabled(level) LogLevel(level) >= Logging::GetMinLevel(MODULE_NAME)

class LogStream;
class LogLevel;

class Logging
{
public:
	static void open(std::string logfile);
	static void close();

	static void SetMinLevel(const char* module, int level);
	static LogLevel GetMinLevel(const char* module);

	static LogStream* instance();

	static const int LOG_LEVEL_ERROR;
	static const int LOG_LEVEL_WARN;

	static const int LOG_LEVEL1;
	static const int LOG_LEVEL2;
	static const int LOG_LEVEL3;
	static const int LOG_LEVEL4;
	static const int LOG_LEVEL5;

	static const int DEFAULT_LOG_LEVEL;

	typedef std::map<std::string, LogLevel> MinLevelsMap;

private:
	static LogStream* _stream;
	static MinLevelsMap _min_levels;
};

class LogStream : public std::fstream
{
public:
	LogStream(std::string logfile);
	~LogStream();

	std::ostream& WriteHeaders(const char* file, int line, const char* func, 
		const char* module, int level);

private:
	std::fstream _os;
};

class StringStream
{
public:
	template <class T> StringStream& operator<<(const T& obj)
	{
		_ss << obj;
		return *this;
	}

	std::string str()
	{
		return _ss.str();
	}

private:
	std::stringstream _ss;
};

class LogLevel
{
public:
	LogLevel()
		: _level(Logging::DEFAULT_LOG_LEVEL) { }
	LogLevel(int level)
		: _level(level) { }

	bool operator>=(LogLevel other)
	{
		return other._level >= _level;
	}

	int Level() { return _level; }

private:
	int _level;
};

#endif

