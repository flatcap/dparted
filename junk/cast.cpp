// g++ -Wall -g -o cast{,.cpp}

#include <stdio.h>

class A
{
public:
	operator int()		{ return 5; }
	operator long()		{ return 6; }
	operator long long()	{ return 7; }
	operator unsigned int()	{ return 8; }
	operator const char *()	{ return "hello world"; }
};

int main (int argc, char *argv[])
{
	A		 a;
	int		 b = a; // No casts required
	long		 c = a;
	long long	 d = a;
	unsigned int	 e = a;
	const	char	*f = a;

	printf ("b = %d\n",   b);
	printf ("c = %ld\n",  c);
	printf ("d = %lld\n", d);
	printf ("e = %d\n",   e);
	printf ("f = %s\n",   f);

	return b;
}

