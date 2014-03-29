/* Copyright (c) 2014 Richard Russon (FlatCap)
 *
 * This file is part of DParted.
 *
 * DParted is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DParted is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DParted.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstdarg>
#include <cstring>
#include <vector>

#include "log.h"
#include "log_trace.h"

//static unsigned int log_level = ~0;
static FILE* file = nullptr;

__attribute__ ((format (printf, 1, 2)))
int
log_debug (const char* format, ...)
{
	if (!file)
		return 0;

	va_list args;
	va_start (args, format);
	//fprintf (file, "\e[38;5;229m");

	std::vector<char> buffer;
	int buf_len = 1024;
	buffer.resize (buf_len);		// Most log lines should be shorter than 1KiB
	buf_len = buffer.size();

	int count = vsnprintf (buffer.data(), buf_len-1, format, args);
	if (count >= buf_len) {
		va_end (args);
		va_start (args, format);
		buffer.resize (count+2);	// Make enough room this time
		buf_len = buffer.size();
		count = vsnprintf (buffer.data(), buf_len-1, format, args);
	}

	fprintf (stdout, "%s", buffer.data());
	//fprintf (file, "\e[0m");
	va_end (args);

	return count;
}

bool
log_init (const char* name)
{
	file = fopen (name, "ae");	// append, close on exec
	//log_info ("log init: %s\n", name);

	if (strncmp (name, "/dev/pts/", 9) == 0) {
		//fprintf (file, "\033c");		// reset
	}

	return (file != nullptr);
}

void
log_close (void)
{
	if (!file)
		return;

	fclose (file);
	file = nullptr;
}


#if 0
unsigned int
log_set_level (unsigned int level)
{
	unsigned int old = log_level;
	log_level = level;
	return old;
}

unsigned int
log_get_level (void)
{
	return log_level;
}

#endif
