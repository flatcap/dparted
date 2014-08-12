#include <iostream>
#include <memory>

#include "plugin.h"

dparted_plugin header = { DP_PLUGIN_MAGIC, DP_PLUGIN_VERSION, &initialise, &uninitialise };

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
}


