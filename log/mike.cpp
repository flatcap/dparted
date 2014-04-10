#include <iostream>
#include <vector>
#include <typeinfo>
#include <cstring>

#include <cxxabi.h>
#include <execinfo.h>

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


/**
 * class Mike
 */
class Mike
{
public:
	Mike (void) = default;
	virtual ~Mike() = default;

	std::vector<std::string> function (int x, char y, std::string s)
	{
		std::cout << __FUNCTION__ << std::endl;
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		return {};
	}
protected:

};


int
main (int argc, char *argv[])
{
	Mike m;
	m.function (0, 0, {});

	std::cout << demangle (typeid(&Mike::function).name()) << std::endl;
	return 0;
}

