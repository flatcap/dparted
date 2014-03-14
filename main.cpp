#include <iostream>

#include "object.h"

int
main()
{
	Object j;

	std::cout << "Names:" << std::endl;
	for (auto n : j.get_prop_names()) {
		std::cout << '\t' << n << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Values:" << std::endl;
	for (auto p : j.get_all_props()) {
		std::cout << '\t' << (std::string) *p << std::endl;
	}
	std::cout << std::endl;

	return 0;
}

