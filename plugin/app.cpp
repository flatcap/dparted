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

	plugin* (*fn_get_plugin_info)(void);
	*(void **) (&fn_get_plugin_info) = dlsym (handle, "get_plugin_info");

	error = dlerror();
	if (error) {
		std::cout << "dlsym failed: " << error << std::endl;
		dlclose (handle);
		return false;
	}

	plugin *header = fn_get_plugin_info();
	if (header) {
		HatstandPtr h (new struct hatstand);
		h->a = 1.414;
		h->b = 0x12345678;
		h->c = 'R';

		WibblePtr w = header->initialise (h);
		std::cout << "init returns" << std::endl;
		std::cout << "\t" << w->x << std::endl;
		std::cout << "\t" << w->y << std::endl;
		std::cout << "\t" << w->z << std::endl;

		w = nullptr;
		h = nullptr;

		header->something();

		ContainerPtr c = header->get_object();

		std::cout << c->get_name() << std::endl;
	}

	dlclose (handle);
	return true;
}

