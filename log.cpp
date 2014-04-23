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
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

#include "log.h"
#include "log_severity.h"
#include "utils.h"

static std::multimap<Severity,log_callback_t> log_mux;
#ifndef DP_LOG_CHECK
static int depth = 0;
#endif

std::mutex log_active;

void
log_stdout (Severity UNUSED(level), const char* UNUSED(function), const char* UNUSED(file), int UNUSED(line), const char* message)
{
	//fprintf (stdout, "%*s", (depth*4), "");	// indent
	fprintf (stdout, "%s\n", message);
}

void
log_stderr (Severity UNUSED(level), const char* UNUSED(function), const char* UNUSED(file), int UNUSED(line), const char* message)
{
	fprintf (stderr, "%s\n", message);
}


#ifdef DP_LOG_CHECK
__attribute__ ((format (printf, 1, 2)))
void
log_redirect (const char* format __attribute__((unused)), ...)
{
}

/**
 * log_redirect (string)
 */
void
log_redirect (const std::string& UNUSED(message))
{
}

/**
 * log_redirect (stringstream)
 */
void
log_redirect (const std::stringstream& UNUSED(message))
{
}

#else
/**
 * log_redirect (message)
 */
void
log_redirect (Severity level, const char* function, const char* file, int line, const char* message)
{
	std::lock_guard<std::mutex> lock (log_active);

#if 0
	std::thread::id thread_id = std::this_thread::get_id();
	std::uint64_t tid = (std::uint64_t) *(reinterpret_cast<std::uint64_t*> (&thread_id));
#endif

	if ((level & Severity::Leave) == Severity::Leave) --depth;

	if (log_mux.empty()) {
		log_stdout (level, function, file, line, message);
	} else {
		for (auto i : log_mux) {
			if ((bool) (i.first & level)) {
				i.second (level, function, file, line, message);
			}
		}
	}

	if ((level & Severity::Enter) == Severity::Enter) ++depth;
}

/**
 * log_redirect (string)
 */
void
log_redirect (Severity level, const char* function, const char* file, int line, const std::string& message)
{
	log_redirect (level, function, file, line, message.c_str());
}

/**
 * log_redirect (stringstream)
 */
void
log_redirect (Severity level, const char* function, const char* file, int line, const std::stringstream& message)
{
	log_redirect (level, function, file, line, message.str().c_str());
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
	log_mux.clear();
}

void
assertion_failure (const char* file, int line, const char* test, const char* function)
{
	std::vector<std::string> bt = get_backtrace();
	log_code ("\033[01;31m%s:%d: assertion failed: (%s) in %s\033[0m", file, line, test, function);
	log_code ("Backtrace:");
	for (auto i : bt) {
		if (i.substr (0, 17) == "assertion_failure")	// Skip me
			continue;
		log_code ("\t%s", i.c_str());
	}
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
