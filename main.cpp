#include <iostream>

#include "app.h"

int
main (int, char*[])
{
	App* a = new App();

	a->initialise();

	delete a;
	return 0;
}
