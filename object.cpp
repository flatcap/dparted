#include <iostream>

class Jim
{
public:
	Jim (void) { me = "this is jim"; }
	~Jim() = default;

protected:
	friend std::ostream& operator<< (std::ostream& stream, const Jim& j);

	std::string me;
};

std::ostream&
operator<< (std::ostream& stream, const Jim& j)
{
	stream << j.me;
	return stream;
}

template<class T>
void
log (const T& x)
{
	std::cout << x << std::endl;
}

int main()
{
	Jim j;
	log (j);
	return 0;
}

