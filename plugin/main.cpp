#include <iostream>

#include "app.h"
#include "plugin.h"

int
main (int, char*[])
{
	App* a = new App();

	a->initialise();

	delete a;
	return 0;
}


