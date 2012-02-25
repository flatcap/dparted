#include <stdio.h>
#include <list>

int main (int argc, char *argv[])
{
	std::list<int> li;
	std::list<int>::iterator it;

	for (int i = 0; i < 20; i++) {
		li.push_back (i);
	}

	printf ("list contains %lu items\n", li.size());

	for (it = li.begin(); it != li.end(); ) {
		if ((*it) == 12) {
			li.erase (it++);
		} else {
			printf ("\t%d\n", (*it));
			++it;
		}
	}

	printf ("list contains %lu items\n", li.size());

	return 0;
}

