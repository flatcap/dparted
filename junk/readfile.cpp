// g++ -Wall -g -o readfile{,.cpp}

#include <iostream>
#include <fstream>
#include <string>

/**
 * main
 */
int main (int argc, char *argv[])
{
	if (argc != 2) {
		std::cout << "Usage: readfile file" << std::endl;
		return 1;
	}

	std::ifstream in (argv[1]);
	std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

	std::cout << "contents:" << std::endl;
	std::cout << contents    << std::endl;

	// Seek back to beginning of file
	in.seekg (0, std::ios_base::beg);

	std::cout << "-------------------------------" << std::endl;

	std::string line;
	if (in.is_open()) {
		while (in.good()) {
			getline (in,line);
			std::cout << line << std::endl;
		}
		in.close();
	} else {
		std::cout << "Unable to open file";
	}


	return 0;
}

