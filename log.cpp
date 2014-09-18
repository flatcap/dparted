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

static std::vector<std::tuple<int, Severity, log_callback_t>> log_mux;
static std::map<std::thread::id, std::uint64_t> thread_list;
static int log_handle = 0;
#ifndef DP_LOG_CHECK
static std::uint64_t fn_depth     = 0;
static std::uint64_t seq_index    = 0;
static std::uint64_t thread_index = 0;
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
	++seq_index;

	std::uint64_t thread_num = 0;

	if ((level & Severity::Leave) == Severity::Leave)
		--fn_depth;

	std::thread::id tid = std::this_thread::get_id();
	if ((level & Severity::ThreadStart) == Severity::ThreadStart) {
		++thread_index;
		thread_list[tid] = thread_index;
		thread_num = thread_index;
	} else {
		thread_num = thread_list[tid];
	}

	if (log_mux.empty()) {
		fprintf (stdout, "%s", message);
	} else {
		for (auto &i : log_mux) {
			if ((bool) (std::get<1>(i) & level)) {
				std::get<2>(i) (seq_index, thread_num, fn_depth, level, function, file, line, message);	// Every log goes through this line
			}
		}
	}

	if ((level & Severity::Enter) == Severity::Enter) {
		++fn_depth;
	}
}

/**
 * log_redirect (string)
 */
void
log_redirect (Severity level, const char* function, const char* file, int line, const std::string& message)
{
	log_redirect (level, function, file, line, SP(message));
}

/**
 * log_redirect (stringstream)
 */
void
log_redirect (Severity level, const char* function, const char* file, int line, const std::stringstream& message)
{
	std::string str = message.str();
	log_redirect (level, function, file, line, SP(str));
}


#endif

int
log_add_handler (log_callback_t cb, Severity s)
{
	++log_handle;
	log_mux.push_back (std::make_tuple (log_handle, s, cb));
	return log_handle;
}

void
log_remove_handler (int handle)
{
	auto end = std::end (log_mux);
	for (auto i = std::begin (log_mux); i != end; ++i) {
		if (std::get<0>(*i) == handle) {
			log_mux.erase(i);
			return;
		}
	}

	log_error ("Can't find log handle: %d", handle);
}

void
assertion_failure (const char* file, int line, const char* test, const char* function)
{
	std::vector<std::string> bt = get_backtrace();
	log_code ("%s:%d: assertion failed: (%s) in %s", file, line, test, function);

	std::stringstream ss;
	ss << "Backtrace:\n";
	for (auto& i : bt) {
		if (i.substr (0, 17) == "assertion_failure") {		//XXX change to __FUNCTION__
			continue;					// Skip me
		}
		ss << "\t" << i << "\n";
	}

	log_code (ss);
}

static std::vector<std::pair<Severity, std::string>> LogLevelNames = {
	{ Severity::SystemEmergency, "SystemEmergency" },
	{ Severity::SystemAlert,     "SystemAlert"     },
	{ Severity::Critical,        "Critical"        },
	{ Severity::Error,           "Error"           },
	{ Severity::Perror,          "Perror"          },
	{ Severity::Code,            "Code"            },
	{ Severity::Warning,         "Warning"         },
	{ Severity::Verbose,         "Verbose"         },
	{ Severity::User,            "User"            },
	{ Severity::Info,            "Info"            },
	{ Severity::Progress,        "Progress"        },
	{ Severity::Quiet,           "Quiet"           },
	{ Severity::Command,         "Command"         },
	{ Severity::Debug,           "Debug"           },
	{ Severity::Trace,           "Trace"           },
	{ Severity::CommandIn,       "CommandIn"       },
	{ Severity::CommandOut,      "CommandOut"      },
	{ Severity::IoIn,            "IoIn"            },
	{ Severity::IoOut,           "IoOut"           },
	{ Severity::Dot,             "Dot"             },
	{ Severity::Hex,             "Hex"             },
	{ Severity::ConfigRead,      "ConfigRead"      },
	{ Severity::ConfigWrite,     "ConfigWrite"     },
	{ Severity::Enter,           "Enter"           },
	{ Severity::Leave,           "Leave"           },
	{ Severity::File,            "File"            },
	{ Severity::Ctor,            "Ctor"            },
	{ Severity::Dtor,            "Dtor"            },
	{ Severity::ThreadStart,     "ThreadStart"     },
	{ Severity::ThreadEnd,       "ThreadEnd"       },
	{ Severity::Utils,           "Utils"           },
	{ Severity::Listener,        "Listener"        }
};

std::string
log_get_level_name (Severity level)
{
	for (auto& i : LogLevelNames) {
		if (i.first == level) {
			return i.second;
		}
	}

	return "UNKNOWN";
}

Severity
log_get_level_value (const std::string& name)
{
	for (auto& i : LogLevelNames) {
		if (i.second == name) {
			return i.first;
		}
	}

	return Severity::NoMessages;
}

