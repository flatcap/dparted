#include <iostream>
#include <memory>

class Jim;

typedef std::shared_ptr<Jim> JimPtr;

class Jim
{
public:
	Jim (void) = default;
	virtual ~Jim() = default;

	friend std::ostream& operator<< (std::ostream& stream, const JimPtr& j);
};

std::ostream&
operator<< (std::ostream& stream, const JimPtr& j)
{
	stream << "Jim(" << (void*)&j << ")";
	return stream;
}

void output (int level)
{
	std::cout << level << ":" << std::endl;
}


template<class T>
void output (int level, std::shared_ptr<T>& j)
{
	std::cout << level << ":" << j << std::endl;
}

void output (int level, const char* message)
{
	std::cout << level << ":" << message << std::endl;
}

template <typename ...A>
void output(int level, const char* format, A ...args)
{
	printf ("%d:", level);
	printf (format, args...);
	printf ("\n");
}

int main()
{
	JimPtr j = std::make_shared<Jim>();
	int level = 42;

	output(level);
	output(level, j);
	output(level, "hello");
	output(level, "hello %d", 99);
	output(level, "hello %d, %f", 99, 3.141);
	output(level, "hello %d, %f, %s", 99, 3.141, "apple");
}

