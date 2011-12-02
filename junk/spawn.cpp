// g++ -o spawn{,.cpp} -Wall -g $(pkg-config --cflags --libs glibmm-2.4)

#include <stdio.h>
#include <glibmm/spawn.h>
#include <math.h>
#include <vector>
#include <string>

/**
 * execute_command
 */
int execute_command (const std::string &command, std::string &output, std::string &error)
{
	int exit_status = -1;
	std::string std_out;
	std::string std_error;

	try
	{
		std::vector<std::string>argv;
		argv.push_back ("sh");
		argv.push_back ("-c");
		argv.push_back (command);

		Glib::spawn_sync (".", argv, Glib::SPAWN_SEARCH_PATH, sigc::slot<void>(), &std_out, &std_error, &exit_status);
	}
	catch  (Glib::Exception & e)
	{
		 error = e.what();
		 return -1;
	}

	output = std_out;
	error = std_error;

	return exit_status;
}

/**
 * main
 */
int main (int argc, char *argv[])
{
	if (argc != 2) {
		printf ("Usage: s program\n");
		return 1;
	}

	std::string command;
	std::string output;
	std::string error;
	int result = 0;

	command = argv[1];

	result = execute_command (command, output, error);

	printf ("command = '%s'\n", command.c_str());
	if (output.size() > 0)
		printf ("output =\n%s\n", output.c_str());
	if (error.size() > 0)
		printf ("error =\n%s\n", error.c_str());

	return result;
}

