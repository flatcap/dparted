// g++ -Wall -g -o compare{,.cpp}

#include <stdio.h>
#include <string>

int main (int argc, char *argv[])
{
	if (argc != 3)
		return 1;

	std::string apple  = argv[1];
	std::string banana = argv[2];

	unsigned int a_len = apple.length();
	//unsigned int b_len = banana.length();

	printf ("comparing: '%s' and '%s'\n", apple.c_str(), banana.c_str());

	int res = apple.compare (0, a_len, banana, 0, a_len);

	printf ("result = %d\n", res);

	return 0;
}

