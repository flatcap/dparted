#include <iostream>

#include "app.h"
#include "dir.h"

App::App (void)
{
}

App::~App()
{
}

void
App::initialise (void)
{
	std::vector<std::string> list = find_plugins ("plugins");

	std::cout << "Plugins:" << std::endl;
	for (auto p : list) {
		std::cout << "\t" << p << std::endl;
	}
}

