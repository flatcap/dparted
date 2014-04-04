#include <exception>
#include <iostream>
#include <cxxabi.h>

struct empty { };

template <typename T, int N>
struct bar { };

int main()
{
	int     status;
	char   *realname;

	// exception classes not in <stdexcept>, thrown by the implementation
	// instead of the user
	std::bad_exception  e;
	realname = abi::__cxa_demangle(e.what(), 0, 0, &status);
	std::cout << e.what() << "\t=> " << realname << "\t: " << status << '\n';
	free(realname);


	// typeid
	bar<empty,17>          u;
	const std::type_info  &ti = typeid(u);

	realname = abi::__cxa_demangle(ti.name(), 0, 0, &status);
	std::cout << ti.name() << "\t=> " << realname << "\t: " << status << '\n';
	free(realname);

	return 0;
}
