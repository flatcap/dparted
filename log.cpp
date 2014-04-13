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
#include <functional>
#include <map>
#include <vector>

#include "log.h"
#include "log_trace.h"
#include "severity.h"
#include "utils.h"

static std::multimap<Severity,log_callback_t> log_mux;

void
log_stdout (Severity UNUSED(level), const char* UNUSED(function), const char* UNUSED(file), int UNUSED(line), const char* message)
{
	fprintf (stdout, "%s", message);
}

void
log_stderr (Severity UNUSED(level), const char* UNUSED(function), const char* UNUSED(file), int UNUSED(line), const char* message)
{
	fprintf (stderr, "%s", message);
}


#ifdef DP_LOG_CHECK
__attribute__ ((format (printf, 1, 2)))
void
log_redirect (const char* format __attribute__((unused)), ...)
{
}

#else
/**
 * log_redirect (message)
 */
void
log_redirect (Severity level, const char* function, const char* file, int line, const char* message)
{
	if (log_mux.empty()) {
		log_stdout (level, function, file, line, message);
	} else {
		for (auto i : log_mux) {
			if ((bool) (i.first & level)) {
				i.second (level, function, file, line, message);
			}
		}
	}
}

#endif

void
log_init (Severity s, log_callback_t cb)
{
	log_mux.insert (std::make_pair(s,cb));
}
void
log_close (void)
{
}

void
assertion_failure (const char* file, int line, const char* test, const char* function)
{
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
