#include <iostream>

#define log_info	set_log(__PRETTY_FUNCTION__,__FILE__,__LINE__); log_method

const char* log_fn   = nullptr;
const char* log_file = nullptr;
int         log_line = 0;

void set_log (const char* fn, const char* file, int line)
{
	log_fn   = fn;
	log_file = file;
	log_line = line;
}

int log_method (const char* format, ...)
{
	return printf ("%s\n", format);
}

std::ostream& log_method (void)
{
	return std::cout;
}

int
main (int argc, char *argv[])
{
	log_info ("format");
	log_info() << "object" << std::endl;

	return 0;
}

