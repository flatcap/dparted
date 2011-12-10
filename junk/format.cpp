#include <string>
#include <vector>

#include <stdio.h>
#include <stdarg.h>

std::string format (const char *format, ...)
{
	std::string result;

	if (format) {
		va_list marker;

		va_start (marker, format);
		size_t len = vsnprintf (NULL, 0, format, marker) + 1;	// +1 for NULL

		va_start (marker, format);
		std::vector<char> buffer (len, '\0');
		int written = vsnprintf (&buffer[0], len, format, marker);

		if (written > 0) {
			result = &buffer[0];
		}

		va_end (marker);
	}

	return result;
}

int main (int argc, char *argv[])
{
	const char *wibble = "wibble";
	std::string jim = format ("%s %g %c\n", wibble, 4.522342343254, 'R');

	printf ("%s", jim.c_str());
	return 0;
}

