#include <iostream>

#include <dlfcn.h>

#include "app.h"
#include "dir.h"
#include "plugin.h"
#include "main.h"

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

	plugin* (*fn_get_info)(void);
	*(void **) (&fn_get_info) = dlsym (handle, "get_plugin_info");

	error = dlerror();
	if (error) {
		std::cout << "dlsym failed: " << error << std::endl;
		dlclose (handle);
		return false;
	}

	plugin *header = fn_get_info();
	if (header) {
		DummyPtr d = header->initialise (&reg);
		std::cout << "init returns " << d->x << std::endl;
		std::cout << "init returns " << d->y << std::endl;
		std::cout << "init returns " << d->z << std::endl;

		d = nullptr;
	}
	std::cout << filename << " returns " << header << std::endl;

	dlclose (handle);
	return true;
}

