#include <iostream>

#include "app.h"
#include "plugin.h"

bool
register_logger (const std::string& name, LoggerPluginPtr lp)
{
	std::cout << "register_logger: " << name << " : " << lp << std::endl;
	return true;
}

bool
register_theme (const std::string& name, ThemePluginPtr tp)
{
	std::cout << "register_theme: " << name << " : " << tp << std::endl;
	return true;
}

bool
register_filesystem (const std::string& name, FilesystemPluginPtr fp)
{
	std::cout << "register_filesystem: " << name << " : " << fp << std::endl;
	return true;
}


register_plugin reg;

int
main (int, char*[])
{
	reg = { register_logger, register_theme, register_filesystem };

	App* a = new App();

	a->initialise();

	delete a;
	return 0;
}


