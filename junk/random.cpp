// g++ -Wall -g -o random{,.cpp}

#include <stdlib.h>
#include <time.h>

#include <iostream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <iterator>

int main (int argc, char *argv[])
{
	srand (time (NULL));

	int a1[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	std::vector<int> v (a1, a1 + 10); // copy of a1
	std::ostream_iterator<int> output (std::cout, " ");

	std::cout << "Vector v before random_shuffle: ";
	std::copy (v.begin(), v.end(), output);

	std::random_shuffle (v.begin(), v.end());
	std::cout << "\nVector v after  random_shuffle: ";
	std::copy (v.begin(), v.end(), output);
	std::cout << std::endl;

	return 0;
}

