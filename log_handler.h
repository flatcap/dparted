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

#ifndef _LOG_HANDLER_H_
#define _LOG_HANDLER_H_

#include <cstdio>
#include <functional>
#include <memory>
#include <string>

#include "log_severity.h"

typedef std::function<void(Severity level, const char* function, const char* file, int line, const char* message)> log_handler_t;

class LogHandler;

typedef std::shared_ptr<LogHandler> LogHandlerPtr;

class LogHandler
{
public:
	static LogHandlerPtr create (const std::string& filename, bool truncate = false);
	static LogHandlerPtr create (FILE* handle);

	virtual ~LogHandler();

	void log_line (Severity level, const char* function, const char* filename, int line, const char* message);

	void flush (void);
	void reset (void);

	bool timestamp      = false;
	bool show_level     = false;
	bool show_file_line = false;
	bool show_function  = false;

	int  foreground = -1;
	int  background = -1;
	bool bold       = false;
	bool underline  = false;
	bool flush_line = false;

protected:
	LogHandler (void);

	FILE* file      = nullptr;
	bool close_file = false;
	bool is_tty     = false;
	std::string filename;
};

#endif // _LOG_HANDLER_H_

