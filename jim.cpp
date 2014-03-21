#include <iostream>
#include <functional>

#define log(X)	_log(__FILE__,__LINE__,(X))

namespace Log {
	void _log (const char* file, int line, const char* message)
	{
		std::cout << file << ":" << line << " -- " << message << std::endl;
	}
}

int
main()
{
	Log::log ("apple");
	int x = 0;
	x++;
	x++;
	Log::log ("banana");
	x++;

	return 0;
}

