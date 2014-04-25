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
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

#include "log.h"
#include "log_severity.h"
#include "utils.h"

static std::vector<std::tuple<int,Severity,log_callback_t>> log_mux;
static int log_handle = 0;
#ifndef DP_LOG_CHECK
static int depth = 0;
#endif

std::mutex log_active;

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

	if ((level & Severity::Leave) == Severity::Leave) --depth;	//XXX add to log callback

	if (log_mux.empty()) {
		fprintf (stdout, "%s", message);
	} else {
		for (auto i : log_mux) {
			if ((bool) (std::get<1>(i) & level)) {
				std::get<2>(i) (level, function, file, line, message);
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

int
log_add_handler (log_callback_t cb, Severity s)
{
	++log_handle;
	log_mux.push_back (std::make_tuple (log_handle,s,cb));
	return log_handle;
}

void
log_remove_handler (int handle)
{
	auto end = std::end (log_mux);
	for (auto i = std::begin (log_mux); i != end; ++i) {
		if (std::get<0>(*i) == handle) {
			log_mux.erase (i);
			return;
		}
	}

	log_error ("Can't find log handle: %d\n", handle);
}

void
assertion_failure (const char* file, int line, const char* test, const char* function)
{
	std::vector<std::string> bt = get_backtrace();
	log_code ("\033[01;31m%s:%d: assertion failed: (%s) in %s\033[0m", file, line, test, function);

	std::stringstream ss;
	ss << "Backtrace:\n";
	for (auto i : bt) {
		if (i.substr (0, 17) == "assertion_failure")	// Skip me
			continue;
		ss << "\t" << i << "\n";
	}

	log_code ("%s", ss.str().c_str());
}


std::string
log_get_level (Severity level)
{
	if (level == Severity::SystemEmergency) return "SystemEmergency";
	if (level == Severity::SystemAlert)     return "SystemAlert";
	if (level == Severity::Critical)        return "Critical";
	if (level == Severity::Error)           return "Error";
	if (level == Severity::Perror)          return "Perror";
	if (level == Severity::Code)            return "Code";
	if (level == Severity::Warning)         return "Warning";
	if (level == Severity::Verbose)         return "Verbose";
	if (level == Severity::User)            return "User";
	if (level == Severity::Info)            return "Info";
	if (level == Severity::Progress)        return "Progress";
	if (level == Severity::Quiet)           return "Quiet";
	if (level == Severity::Command)         return "Command";
	if (level == Severity::Debug)           return "Debug";
	if (level == Severity::Trace)           return "Trace";
	if (level == Severity::CommandIn)       return "CommandIn";
	if (level == Severity::CommandOut)      return "CommandOut";
	if (level == Severity::IoIn)            return "IoIn";
	if (level == Severity::IoOut)           return "IoOut";
	if (level == Severity::Dot)             return "Dot";
	if (level == Severity::Hex)             return "Hex";
	if (level == Severity::ConfigRead)      return "ConfigRead";
	if (level == Severity::ConfigWrite)     return "ConfigWrite";
	if (level == Severity::Enter)           return "Enter";
	if (level == Severity::Leave)           return "Leave";
	if (level == Severity::File)            return "File";
	if (level == Severity::Ctor)            return "Ctor";
	if (level == Severity::Dtor)            return "Dtor";
	if (level == Severity::Thread)          return "Thread";

	return "UNKNOWN";
}


