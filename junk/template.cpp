#include <sstream>
#include <iostream>

template <class T>
std::string dump_row (const T& value)
{
	std::stringstream s;

	s << "<tr><td>" << value << "</td></tr>";

	return s.str();
}

int main (int argc, char *argv[])
{
	std::string output;
	std::string input1 ("hello");
	int input2 = 42;
	double input3 = 3.1415926;

	output = dump_row (input1);
	std::cout << "output " << output << std::endl;

	output = dump_row (input2);
	std::cout << "output " << output << std::endl;

	output = dump_row (input3);
	std::cout << "output " << output << std::endl;

	return 0;
}
