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

#ifndef _LOG_H_
#define _LOG_H_

#include <memory>
#include <sstream>
#include <vector>

#include "log_severity.h"
#include "log_macro.h"
#include "log_handler.h"

typedef std::function<void(std::uint64_t index, std::uint64_t thread_id, std::uint64_t fn_depth, Severity level, const char* function, const char* file, int line, const char* message)> log_callback_t;

#ifdef DP_LOG_CHECK
void log_redirect (const char* format __attribute__((unused)), ...) __attribute__((format (printf, 1, 2)));
void log_redirect (const std::string& message);
void log_redirect (const std::stringstream& message);
#else
void log_redirect (Severity level, const char* function, const char* file, int line, const char* message);
void log_redirect (Severity level, const char* function, const char* file, int line, const std::string& message);
void log_redirect (Severity level, const char* function, const char* file, int line, const std::stringstream& message);

template<class T>
void
log_redirect (Severity level, const char* function, const char* file, int line, std::shared_ptr<T>& j)
{
	std::stringstream ss;
	ss << j;
	log_redirect (level, function, file, line, ss.str().c_str());
}

template <typename ...A>
void
log_redirect (Severity level, const char* function, const char* file, int line, const char* format, A ...args)
{
	std::vector<char> buffer;
	int buf_len = 1024;
	buffer.resize (buf_len);		// Most log lines should be shorter than 1KiB

	int count = snprintf (buffer.data(), buf_len, format, args...);
	if (count >= buf_len) {
		buffer.resize (count+2);	// Make enough room this time
		count = snprintf (buffer.data(), count+1, format, args...);
	}

	log_redirect (level, function, file, line, buffer.data());
}

#endif

void assertion_failure (const char* file, int line, const char* test, const char* function);

#define return_if_fail(TEST)									\
	do {											\
		if (TEST) {									\
		} else {									\
			assertion_failure (__FILE__, __LINE__, #TEST, __PRETTY_FUNCTION__);	\
			return;									\
		}										\
	} while (0)

#define return_val_if_fail(TEST,VALUE)								\
	do {											\
		if (TEST) {									\
		} else {									\
			assertion_failure (__FILE__, __LINE__, #TEST, __PRETTY_FUNCTION__);	\
			return VALUE;								\
		}										\
	} while (0)

int log_add_handler (log_callback_t cb, Severity s);
void log_remove_handler (int handle);

std::string log_get_level_name  (Severity level);
Severity    log_get_level_value (const std::string& name);

#include "log_trace.h"

#endif // _LOG_H_

