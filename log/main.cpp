#include <iostream>

#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))

int main (int UNUSED(argc), char **UNUSED(argv))
{
	std::cout << "hello world" << std::endl;
	return 0;
}

