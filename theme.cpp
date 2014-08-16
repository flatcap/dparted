#include <iostream>
#include <memory>

#include "plugin.h"

ThemePluginPtr my_theme;

int initialise (register_plugin* reg);
void uninitialise (void);

plugin header = { PLUGIN_MAGIC, PLUGIN_VERSION, &initialise, &uninitialise };

std::string
get_colour (const std::string&)
{
	return "pink";
}

std::string
get_icon (const std::string&)
{
	return "/path/to/icon.ico";
}

int
initialise (register_plugin* reg)
{
	std::cout << "Theme plugin initialised" << std::endl;

	ThemePluginPtr t (new ThemePlugin {get_colour, get_icon});
	my_theme = t;

	// reg->register_theme ("Psychedelic", my_theme);
	return 42;
}

void
uninitialise (void)
{
	std::cout << "Theme plugin uninitialised" << std::endl;
	my_theme = nullptr;
}


extern "C" {

plugin*
get_plugin_info (void)
{
	return &header;
}

}

