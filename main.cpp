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

	std::cout << "Types:" << std::endl;
	for (auto p : j.get_all_props()) {
		std::cout << '\t' << p->get_type_name() << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Values:" << std::endl;
	for (auto p : j.get_all_props()) {
		std::cout << '\t' << (std::string) *p << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Casts:" << std::endl;
	std::cout << "\tstring: " << (std::string) *j.get_prop ("bytes_size") << std::endl;
	std::cout << "\tlong:   " << (long)   *j.get_prop ("bytes_size") << std::endl;
	std::cout << std::endl;

	std::cout << "Helpers:" << std::endl;
	std::cout << '\t' << j.get_uuid_short()         << std::endl;
	std::cout << '\t' << j.get_device_short()       << std::endl;
	std::cout << '\t' << j.get_device_major_minor() << std::endl;
	std::cout << '\t' << j.get_bytes_size_human()   << std::endl;
	std::cout << '\t' << j.get_bytes_free_human()   << std::endl;
	std::cout << std::endl;

	return 0;
}

