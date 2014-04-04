#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include <cxxabi.h>
#include <execinfo.h>

std::vector<std::string> my_bt;

std::string
demangle (const char* symbol)
{
	std::vector<char> buffer;
	buffer.resize (256);	// Most prototypes should be shorter than this

	char* demangled = nullptr;

	// C++ stack: demangle2(_Z7mytracev+0x20) [0x4019f1]
	if (sscanf (symbol, "%*[^(]%*[^_]%255[^)+]", buffer.data()) == 1) {
		if ((demangled = abi::__cxa_demangle (buffer.data(), NULL, NULL, NULL))) {
			std::string result (demangled);
			free (demangled);
			return result;
		}
	}

	// Typeinfo: 3barI5emptyLi17EE
	if ((demangled = abi::__cxa_demangle (symbol, NULL, NULL, NULL))) {
		std::string result (demangled);
		free (demangled);
		return result;
	}

	// C stack: demangle2(main+0x19) [0x401bc8]
	if (sscanf (symbol, "%*[^(](%255[^)+]", buffer.data()) == 1) {
		if (strncmp (buffer.data(), "main", 4) == 0) {
			return "main(int, char**)";
		} else {
			return buffer.data();
		}
	}

	return symbol;
}

std::vector<std::string>
get_backtrace (bool reverse = true)
{
	const int MAX_FRAMES = 100;
	std::vector<std::string> bt;

	void* addresses[MAX_FRAMES];
	int size = backtrace (addresses, MAX_FRAMES);
	char** symbols = backtrace_symbols (addresses, size);
	for (int x = 1; x < size; ++x) {			// Skip 0 (ourself)
		std::string sym = demangle (symbols[x]);
		bt.push_back (sym);
		if (sym.substr (0, 4) == "main")		// Just libc after this
			break;
	}
	free (symbols);
	if (reverse) {
		std::reverse (std::begin (bt), std::end (bt));
	}
	return bt;
}

std::vector<std::string>
c (int w, const char *msg)
{
	(void) w;
	(void) msg;
	my_bt = get_backtrace();

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

	for (auto i : my_bt) {
		std::cout << i << std::endl;
	}

	return 0;
}


