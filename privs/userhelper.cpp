#include <iostream>

#include <unistd.h>
#include <sys/types.h>

int
main (int argc, char *argv[])
{
	std::cout << getuid() << std::endl;
	std::cout << geteuid() << std::endl;
	std::cout << getgid() << std::endl;
	std::cout << getegid() << std::endl;

	const char* old_uid (getenv ("USERHELPER_UID"));
	if (old_uid) {
		std::cout << old_uid << std::endl;
	}
	return 0;
}

