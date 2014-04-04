// g++ -g -std=c++11 -Wall -Wextra -Wpedantic -o demangle{,.cpp} -rdynamic && demangle

#include <iostream>
#include <string>
#include <cxxabi.h>
#include <execinfo.h>
#include <vector>

std::string
demangle(const char* symbol) {
	size_t size;
	int status;
	char temp[128];
	char* demangled;
	//first, try to demangle a c++ name
	if (1 == sscanf(symbol, "%*[^(]%*[^_]%127[^)+]", temp)) {
		if (NULL != (demangled = abi::__cxa_demangle(temp, NULL, &size, &status))) {
			std::string result(demangled);
			free(demangled);
			return result;
		}
	}
	//if that didn't work, try to get a regular c symbol
	if (1 == sscanf(symbol, "%127s", temp)) {
		return temp;
	}

	//if all else fails, just return the symbol
	return symbol;
}

#define MAX_FRAMES 100
void mytrace() {
	void* addresses[MAX_FRAMES];
	int size;
	size = backtrace(addresses, MAX_FRAMES);
	char** symbols = backtrace_symbols(addresses, size);
	int x;
	for (x = 0; x < size; ++x) {
		printf("%s\n", demangle(symbols[x]).c_str());
	}
	free(symbols);
}

std::vector<std::string> c (int w, const char *msg)
{
	(void) w;
	(void) msg;
	mytrace();

	return {};
}

double b (int x, int y)
{
	(void) x;
	c (y, "hello");

	return 3.14159;
}

std::string a (int x)
{
	b (x, -x);

	return std::string ("message");
}

int main()
{
	a(42);
	return 0;
}

