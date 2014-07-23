#include <memory>
#include <ctime>
#include <functional>

#include "app.h"

int
main()
{
	srandom (time (nullptr));

	app = std::make_shared<App>();
	app->test();
	app = nullptr;

	return 0;
}

