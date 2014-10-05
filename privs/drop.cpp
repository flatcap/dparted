#include <iostream>

#include <unistd.h>
#include <sys/types.h>

int
main (int argc, char *argv[])
{
	setegid (1000);
	seteuid (6);

	unsigned int ruid, euid, suid, rgid, egid, sgid;

	getresuid(&ruid, &euid, &suid);
	getresgid(&rgid, &egid, &sgid);

	std::cout << ruid << " " << euid << " " << suid << std::endl;
	std::cout << rgid << " " << egid << " " << sgid << std::endl;
	return 0;
}

