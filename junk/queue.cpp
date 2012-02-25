#include <stdio.h>
#include <queue>

class jim
{
public:
	jim (int num) : value (num)
	{
		printf ("ctor jim (%d)\n", value);
	}

	jim (const jim &j)
	{
		value = j.value;
		printf ("copy jim (%d)\n", value);
	}

	~jim()
	{
		printf ("dtor jim (%d)\n", value);
	}

	int value;
};

int main (int argc, char *argv[])
{
	std::queue<jim> qi;
	std::queue<jim> qj;

	for (int i = 0; i < 10; i++) {
		jim j (i);
		qi.push (j);
	}

	printf ("queue i contains %lu items\n", qi.size());
	printf ("queue j contains %lu items\n", qj.size());

	for (; !qi.empty(); qi.pop()) {
		jim k = qi.front();

		if (k.value % 2)
			qj.push (k);

		printf ("\t%d\n", k.value);
	}

	printf ("queue i contains %lu items\n", qi.size());
	printf ("queue j contains %lu items\n", qj.size());

	return 0;
}

