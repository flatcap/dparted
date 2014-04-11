#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

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
	std::vector<char> buffer;
	int buf_len = 1024;
	buffer.resize (buf_len);		// Most log lines should be shorter than 1KiB

	int count = snprintf (buffer.data(), buf_len, format, args...);
	if (count >= buf_len) {
		buffer.resize (count+2);	// Make enough room this time
		count = snprintf (buffer.data(), count+1, format, args...);
	}

	return output (level, function, file, line, buffer.data());
}


#define log_debug(...) output(42, __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_trace(...) output(99, __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)

int main()
{
	JimPtr j = std::make_shared<Jim>();

	log_debug (j);
	log_debug ("hello");
	log_trace ("hello %d", 99);
	log_debug ("hello %d, %f", 99, 3.141);
	log_trace ("hello %d, %f, %s", 99, 3.141, "apple");
}

