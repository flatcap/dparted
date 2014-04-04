#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include <cxxabi.h>
#include <execinfo.h>

#define MAX_FRAMES 100

std::string
demangle (const char* symbol)
{
	size_t size;
	int status;
	char temp[128];
	char* demangled;

	// C++ stack: demangle2(_Z7mytracev+0x20) [0x4019f1]
	if (sscanf (symbol, "%*[^(]%*[^_]%127[^)+]", temp) == 1) {
		if (NULL != (demangled = abi::__cxa_demangle (temp, NULL, &size, &status))) {
			std::string result (demangled);
			free (demangled);
			return result;
		}
	}

	// Typeinfo: 3barI5emptyLi17EE
	if ((demangled = abi::__cxa_demangle (symbol, NULL, &size, &status)) != NULL) {
		std::string result (demangled);
		free (demangled);
		return result;
	}

	// C stack: demangle2(main+0x19) [0x401bc8]
	if (sscanf (symbol, "%*[^(](%127[^)+]", temp) == 1) {
		if (strncmp (temp, "main", 4) == 0)
			return "main(int, char**)";
		else
			return temp;
	}

	return symbol;
}

void
mytrace (void)
{
	void* addresses[MAX_FRAMES];
	int size = backtrace (addresses, MAX_FRAMES);
	char** symbols = backtrace_symbols (addresses, size);
	for (int x = 0; x < size; ++x) {
		std::string sym = demangle (symbols[x]);
		printf ("%s\n", demangle (symbols[x]).c_str());

		if (sym.substr (0, 4) == "main")
			break;
	}
	free (symbols);
}

std::vector<std::string>
c (int w, const char *msg)
{
	(void) w;
	(void) msg;
	mytrace();

	return {};
}

double
b (int x, int y)
{
	(void) x;
	c (y, "hello");

	return 3.14159;
}

std::string
a (int x)
{
	b (x, -x);

	return std::string ("message");
}

int
main()
{
	a (42);
	return 0;
}


