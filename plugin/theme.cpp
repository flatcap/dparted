#include <iostream>
#include <memory>

#include "plugin.h"
#include "disk.h"

WibblePtr initialise (HatstandPtr);
void something (void);
ContainerPtr get_object (void);

HatstandPtr my_h;

plugin header = { 0x1234, initialise, something, get_object };

WibblePtr
initialise (HatstandPtr h)
{
	std::cout << "Theme::initialise" << std::endl;
	std::cout << "\t" << h->a << std::endl;
	std::cout << "\t" << h->b << std::endl;
	std::cout << "\t" << h->c << std::endl;

	my_h = h;

	WibblePtr d (new struct wibble);
	d->x = 67;
	d->y = 3.141;
	d->z = 123456;
	return d;
}

void
something (void)
{
	std::cout << "Theme::something" << std::endl;
	std::cout << "\t" << my_h->a << std::endl;
	std::cout << "\t" << my_h->b << std::endl;
	std::cout << "\t" << my_h->c << std::endl;

	my_h = nullptr;
}

ContainerPtr
get_object (void)
{
	DiskPtr d (new Disk);
	ContainerPtr c(d);
	return c;
}

extern "C" {
plugin*
get_plugin_info (void)
{
	return &header;
}

}

