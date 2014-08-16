#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include <string>
#include <memory>
#include <functional>

#include "container.h"

struct wibble {
	int x;
	double y;
	long z;
};

struct hatstand {
	float a;
	long long b;
	char c;
};

typedef std::shared_ptr<struct wibble>   WibblePtr;
typedef std::shared_ptr<struct hatstand> HatstandPtr;

typedef struct
{
	std::uint32_t magic;
	std::function<WibblePtr(HatstandPtr)> initialise;
	std::function<void(void)> something;
	std::function<ContainerPtr(void)> get_object;
} plugin;

#endif // _PLUGIN_H_


