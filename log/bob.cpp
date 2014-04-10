#include <iostream>

#define LOG(ARGS...)	log_method (__PRETTY_FUNCTION__,__FILE__,__LINE__,##ARGS)
#define LOG		log_method (__PRETTY_FUNCTION__,__FILE__,__LINE__)

void log_method (const char* fn, const char* file, int line, const char* format, ...)
{
	std::cout << "format" << std::endl;
}

void log_method (const char* fn, const char* file, int line)
{
	std::cout << "object" << std::endl;
}

int
main (int argc, char *argv[])
{
	const char* rcpt = "world";

	LOG ("hello %s", rcpt);
	LOG;

	return 0;
}

