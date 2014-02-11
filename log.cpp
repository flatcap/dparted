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
#include <cstdio>
#include <cstring>
#include <iostream>

#include "log.h"
#include "log_trace.h"

//static unsigned int log_level = ~0;
static FILE* file = nullptr;

/**
 * log
 */
__attribute__ ((format (printf, 1, 0)))
static int
log (const char* format, va_list args)
{
	char buffer[1024];

	if (!file)
		return 0;

	int count = vsnprintf (buffer, sizeof (buffer), format, args);
	//XXX check count against buffer size

	std::cout << buffer << std::flush;

	return count;
}


/**
 * log_debug
 */
__attribute__ ((format (printf, 1, 2)))
int
log_debug (const char* format, ...)
{
	va_list args;
	int retval;

	if (!file)
		return 0;

	va_start (args, format);
	//fprintf (file, "\e[38;5;229m");
	retval = log (format, args);
	//fprintf (file, "\e[0m");
	va_end (args);

	return retval;
}

/**
 * log_error
 */
__attribute__ ((format (printf, 1, 2)))
int
log_error (const char* format, ...)
{
	va_list args;
	int retval;

	if (!file)
		return 0;

	va_start (args, format);
	//fprintf (file, "\033[01;31m");
	retval = log (format, args);
	//fprintf (file, "\033[0m");
	va_end (args);

	return retval;
}

/**
 * log_info
 */
__attribute__ ((format (printf, 1, 2)))
int
log_info (const char* format, ...)
{
	va_list args;
	int retval;

	if (!file)
		return 0;

	va_start (args, format);
	//fprintf (file, "\e[38;5;79m");
	retval = log (format, args);
	//fprintf (file, "\e[0m");
	va_end (args);

	return retval;
}

/**
 * log_trace
 */
__attribute__ ((format (printf, 1, 2)))
int
log_trace (const char* format, ...)
{
	va_list args;
	int retval;

	if (!file)
		return 0;

	va_start (args, format);
	//fprintf (file, "\033[38;5;70m");
	retval = log (format, args);
	//fprintf (file, "\033[0m");
	va_end (args);
	fflush (file);

	return retval;
}


/**
 * log_init
 */
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

/**
 * log_close
 */
void
log_close (void)
{
	if (!file)
		return;
	fclose (file);
	file = nullptr;
}


#if 0
/**
 * log_set_level
 */
unsigned int
log_set_level (unsigned int level)
{
	unsigned int old = log_level;
	log_level = level;
	return old;
}

/**
 * log_get_level
 */
unsigned int
log_get_level (void)
{
	return log_level;
}

#endif
