// g++ -Wall -g -o inherit{,.cpp}

#include <string>
#include <stdio.h>
#include <stdlib.h>

class CString : public std::string
{
public:
	CString (const char *str) :
		std::string (str)
	{
	}

	operator long()
	{
		return strtol (c_str(), NULL, 10);
	}
};

int main (int argc, char *argv[])
{
	CString s ("1234");
	long num = s;

	printf ("s is '%s'\n", s.c_str());
	printf ("s is %lu chars long\n", s.size());
	printf ("num = %ld\n", num);

	return 0;
}

