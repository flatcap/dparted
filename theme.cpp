#include <iostream>
#include <memory>

extern "C" {
int
initialise (void)
{
	return 42;
}
}

#if 0
#include "plugin.h"

dparted_plugin header = { DP_PLUGIN_MAGIC, DP_PLUGIN_VERSION, &initialise, &uninitialise };

theme_plugin plug = { &get_colour, &get_icon };

dparted_plugin
get_dparted_plugin_info (void)
{
	return header;
}

void
initialise (register_plugin* reg)
{
	std::cout << "Theme plugin initialised" << std::endl;
	reg->register_theme (tp);
}

void
uninitialise (void)
{
	std::cout << "Theme plugin uninitialised" << std::endl;
};

Gdk::RGBA
get_colour (const std::string& name)
{
	return 0;
}

Glib::RefPtr<Gdk::Pixbuf>
get_icon (const std::string& name)
{
	return nullptr;
}
#endif

