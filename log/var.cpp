#include <iostream>
#include <cstdarg>
#include <cstring>
#include <cstdio>
#include <vector>

#define LOG(...) log(__LINE__,__VA_ARGS__)

int
log (int line, ...)
{
	va_list args;
	va_start (args, line);

	int count = 0;
	const char *format = va_arg(args,const char*);

	std::vector<char> buffer;
	int buf_len = 1024;
	buffer.resize (buf_len);		// Most log lines should be shorter than 1KiB
	buf_len = buffer.size();

	count = vsnprintf (buffer.data(), buf_len-1, format, args);
	fprintf (stdout, "%s", buffer.data());
	va_end (args);

	return count;
}

int
main()
{
	LOG ("hello %d\n", 42);
	LOG ("world\n");

	return 0;
}
