#include <iostream>
#include <typeinfo>
#include <stdlib.h>

#include "log.h"
#include "hless.h"
#include "screen.h"
#include "error.h"
#include "plain_file.h"
#include "cache.h"

static __attribute__((unused)) const char* MODULE_NAME = "main";

void print_usage()
{
	std::cout << "Usage: hless <filename>" << std::endl;
}

int main(int argc, char* argv[])
{
	if (argc != 2) {
		print_usage();
		exit(-1);
	}

	Logging::open("log.txt");

	Logging::SetMinLevel("main", Logging::LOG_LEVEL1);
	Logging::SetMinLevel("hless", Logging::LOG_LEVEL1);
	Logging::SetMinLevel("screen", Logging::LOG_LEVEL1);
	Logging::SetMinLevel("buffer", Logging::LOG_LEVEL3);
	Logging::SetMinLevel("file", Logging::LOG_LEVEL1);
	Logging::SetMinLevel("cache", Logging::LOG_LEVEL1);
	Logging::SetMinLevel("plain", Logging::LOG_LEVEL1);
	Logging::SetMinLevel("input", Logging::LOG_LEVEL1);
	Logging::SetMinLevel("brush", Logging::LOG_LEVEL3);
	Logging::SetMinLevel("line", Logging::LOG_LEVEL_WARN);
	Logging::SetMinLevel("chunk", Logging::LOG_LEVEL1);
	Logging::SetMinLevel("status", Logging::LOG_LEVEL1);

	Log1("Hello and welcome to hless log file!!!");

	try {
		PlainFile file;
		Cache cache(file);
		Buffer buffer(cache);

		file.open(argv[1]);

		Hless hless(buffer);

		hless.run();

		file.close();
	} catch (std::exception& ex) {
		std::cerr << ex.what();
	}

	Logging::close();
	return 0;
}

