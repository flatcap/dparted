#include <iostream>
#include <string>
#include <vector>

#include <sys/types.h>
#include <dirent.h>

std::vector<std::string>
find_plugins (const std::string& directory)
{
	std::vector<std::string> names;

	DIR *dir = nullptr;
	struct dirent *ent = nullptr;

	dir = opendir (directory.c_str());
	if (!dir) {
		std::cout << "Can't open directory: " << directory << std::endl;
		return {};
	}

	while ((ent = readdir (dir))) {
		std::string file = ent->d_name;
		std::size_t len = file.size();
		if (len < 4) {
			continue;
		}

		std::size_t pos = file.find (".so");
		if (pos == std::string::npos) {
			continue;
		}

		if (pos != (len-3)) {
			continue;
		}

		names.push_back (directory + "/" + file);
	}
	closedir (dir);

	return names;
}

