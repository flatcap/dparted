#include <iostream>

class Log : public std::ostream
{
public:
	int format (const char* msg)
	{
		std::cout << msg << std::endl;

		return 42;
	}
};

int
main()
{
	Log l (std::cout);

	l.format ("hello");
	l << "object" << std::endl;

	return 0;
}

