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
#include <functional>

#include "log.h"
#include "log_trace.h"
#include "utils.h"

//static unsigned int log_level = ~0;
static FILE* file = nullptr;

int
log_redirect (Severity UNUSED(sev), const char* UNUSED(function), const char* file, int line, ...)
{
	if (!file)
		return 0;

	va_list args;
	va_start (args, line);
	const char *format = va_arg(args,const char*);
	//fprintf (file, "\e[38;5;229m");

	std::vector<char> buffer;
	int buf_len = 1024;
	buffer.resize (buf_len);		// Most log lines should be shorter than 1KiB
	buf_len = buffer.size();

	int count = vsnprintf (buffer.data(), buf_len-1, format, args);
	if (count >= buf_len) {
		// log_debug ("overflow (%d > %d)\n", count, buf_len);
		va_end (args);
		va_start (args, line);
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

void
assertion_failure (const char* file, int line, const char* test, const char* function)
{
	//XXX use log_redirect and bind to preserve file/line/function

#if 0
	auto fn = std::bind (&log_redirect, Severity::Code, function, file, line, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6);
	fn ("hello %d\n", 42, 43, 44, 45, 46);
#endif

	std::vector<std::string> bt = get_backtrace();
	log_code ("%s:%d: assertion failed: (%s) in %s\n", file, line, test, function);
	log_code ("Backtrace:\n");
	for (auto i : bt) {
		if (i.substr (0, 17) == "assertion_failure")	// Skip me
			continue;
		log_code ("\t%s\n", i.c_str());
	}
	log_code ("\n");
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
