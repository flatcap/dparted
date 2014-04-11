#include <iostream>
#include <memory>
#include <string>
#include <sstream>

class Jim;

typedef std::shared_ptr<Jim> JimPtr;

/**
 * class Jim
 */
class Jim
{
public:
	Jim (void) = default;
	virtual ~Jim() = default;

	friend std::ostream& operator<< (std::ostream& stream, const JimPtr& j);
};

std::ostream& operator<< (std::ostream& stream, const JimPtr& j)
{
	stream << "Jim(" << (void*)&j << ")";
	return stream;
}


/**
 * output (message)
 */
int output (int level, const char* function, const char* file, int line, const char* message)
{
	std::cout << "Log:" << std::endl;
	std::cout << '\t' << level << std::endl;
	std::cout << '\t' << function << std::endl;
	std::cout << '\t' << file << ":" << line << std::endl;
	std::cout << '\t' << message << std::endl;
	return 123;
}

/**
 * output (object)
 */
template<class T>
int output (int level, const char* function, const char* file, int line, std::shared_ptr<T>& j)
{
	std::stringstream ss;
	ss << j;
	return output (level, function, file, line, ss.str().c_str());
}

/**
 * object (format,...)
 */
template <typename ...A>
int output(int level, const char* function, const char* file, int line, const char* format, A ...args)
{
	printf ("%d:", level);
	//printf (format, args...);
	int buflen = 1000;
	char buffer[1000];
	//int count =
	snprintf (buffer, buflen, format, args...);
	return output (level, function, file, line, buffer);
	//fprintf (stderr, "count = %d\n", count);
#if 0
	if (count < 10) {
		std::cout << buffer;
	} else {
		char buf2[100];
		count = snprintf (buf2, sizeof(buf2), format, args...);
		//fprintf (stderr, "count = %d\n", count);
		std::cout << buf2;
	}
#endif
	printf ("\n");
}


#define log_object(level,object) output(level,__PRETTY_FUNCTION__,__FILE__,__LINE__,object)
#define log_debug(...) output(42, __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_trace(...) output(99, __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)

int main()
{
	JimPtr j = std::make_shared<Jim>();
	int level = 42;

	log_object (level, j);
	log_debug  ("hello");
	log_trace  ("hello %d", 99);
	log_debug  ("hello %d, %f", 99, 3.141);
	log_trace  ("hello %d, %f, %s", 99, 3.141, "apple");
}

