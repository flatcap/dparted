#include <stdio.h>

int main (int argc, char *argv[])
{
	printf ("sizeof (char)      = %d\n", sizeof (char));
	printf ("sizeof (int)       = %d\n", sizeof (int));
	printf ("sizeof (long)      = %d\n", sizeof (long));
	printf ("sizeof (long long) = %d\n", sizeof (long long));
	printf ("sizeof (size_t)    = %d\n", sizeof (size_t));

	return 0;
}

