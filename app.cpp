#include <iostream>
#include <functional>

#include <dlfcn.h>

#include "app.h"
#include "dir.h"

typedef std::function<int(void)> init_t;

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

	if (list.empty())
		return;

	std::cout << "Plugins:" << std::endl;
	for (auto p : list) {
		std::cout << "\t" << p << std::endl;
	}
	std::cout << std::endl;

	for (auto p : list) {
		start_plugin(p);
	}
}

bool
App::start_plugin (const std::string& filename)
{
	void* handle = nullptr;
	char* error  = nullptr;

	handle = dlopen (filename.c_str(), RTLD_NOW);
	if (!handle) {
		std::cout << "dlopen failed: " << dlerror() << std::endl;
		return false;
	}

	int (*fn_initialise)(void);
	*(void **) (&fn_initialise) = dlsym(handle, "initialise");

	error = dlerror();
	if (error) {
		std::cout << "dlsym failed: " << error << std::endl;
		dlclose (handle);
		return false;
	}

	std::cout << filename << " returns " << fn_initialise() << std::endl;

	dlclose (handle);
	return true;
}

