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

#ifndef _LOG_TRACE_H_
#define _LOG_TRACE_H_

#include <string>
#include <thread>

#include "log_macro.h"

//XXX keep a global depth counter?
//XXX indent results?

#ifdef DEBUG
#define LOG_TRACE		LogTrace __le(__PRETTY_FUNCTION__, __FILE__, __LINE__)
#define LOG_THREAD		LogTrace __lt(__PRETTY_FUNCTION__, __FILE__, __LINE__, "thread")
#else
#define LOG_TRACE		do {} while (0)
#define LOG_THREAD		do {} while (0)
#endif

class LogTrace
{
public:
	LogTrace (const std::string& fn, const std::string& file, int line, const char *suf = nullptr) :
		function (fn),
		file_name (file),
		line_num (line)
	{
		if (suf)
			suffix = std::string (" ") + suf;

		std::thread::id thread_id = std::this_thread::get_id();
		std::uint64_t tid = (std::uint64_t) *(reinterpret_cast<std::uint64_t*> (&thread_id));
		log_enter ("Entering%s %s (%ld) -- %s:%d", suffix.c_str(), function.c_str(), tid, file_name.c_str(), line_num);
	}

	virtual ~LogTrace()
	{
		std::thread::id thread_id = std::this_thread::get_id();
		std::uint64_t tid = (std::uint64_t) *(reinterpret_cast<std::uint64_t*> (&thread_id));
		log_leave ("Leaving%s  %s (%ld) -- %s", suffix.c_str(), function.c_str(), tid, file_name.c_str());
	}

protected:
	std::string	function;
	std::string	file_name;
	std::string	suffix;
	int		line_num;

private:
	// Stop users dynamically creating this object
	void* operator new (std::size_t s);
};


#endif // _LOG_TRACE_H_

