#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include <string>
#include <vector>

/**
 * execute_command
 */
unsigned int execute_command (const std::string &command, std::vector<std::string> &output)
{
	FILE *file = NULL;
	char *ptr = NULL;
	size_t n = 0;
	int count = 0;

	output.clear();

	// Execute command and save its output to stdout
	file = popen (command.c_str(), "r");
	if (file == NULL) {
		perror ("popen failed");
		return -1;
	}

	do {
		ptr = NULL;
		n = 0;
		count = getline (&ptr, &n, file);
		output.push_back (ptr);
		free (ptr);
	} while (count > 0);


	if (pclose (file) == -1) {
		perror ("pclose failed");
		return -1;
	}

	return output.size();
}

/**
 * main
 */
int main (int argc, char *argv[])
{
	int lines = 0;
	std::string command;
	std::vector<std::string> output;
	std::vector<std::string>::iterator it;

	if (argc != 2)
		return 1;

	command = argv[1];

	lines = execute_command (command, output);
	if (lines == 0)
		return 1;

	for (it = output.begin(); it != output.end(); it++) {
		printf ("%s", (*it).c_str());
	}

	return 0;
}
