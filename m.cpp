#include <stdio.h>
#include <map>
#include <string>

class Jim
{
public:
	std::string	name;
	int		value;
};

int main (int argc, char *argv[])
{
	Jim anteater;
	Jim baboon;
	Jim cat;
	Jim dog;
	Jim echidna;

	anteater.name = "forty-two";    anteater.value = 42;
	baboon.name   = "twenty-two";   baboon.value   = 22;
	cat.name      = "seventy-nine"; cat.value      = 79;
	dog.name      = "eleven";       dog.value      = 11;
	echidna.name  = "six";          echidna.value  =  6;

	std::map<const char *, Jim*> m;

	m["anteater"] = &anteater;
	m["baboon"]   = &baboon;
	m["cat"]      = &cat;
	m["dog"]      = &dog;
	m["echidna"]  = &echidna;

	Jim *jp = m["baboon"];

	printf ("%s = %d\n", jp->name.c_str(), jp->value);

	return 0;
}

