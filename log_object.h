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

#ifndef _LOG_OBJECT_H_
#define _LOG_OBJECT_H_

#include <string>
#include <cstdio>

#include "severity.h"

class LogObject
{
public:
	LogObject();
	virtual ~LogObject();

	bool open_file (const std::string& filename, bool truncate = false);

	bool timestamp      = false;
	bool show_level     = false;
	bool show_file_line = false;
	bool show_function  = false;
	bool reset_tty      = false;

	int  foreground = -1;
	int  background = -1;
	bool bold       = false;
	bool underline  = false;

	void log_line (Severity level, const char* function, const char* file, int line, const char* message);

protected:
	FILE *file = nullptr;
	std::string filename;
};

#endif // _LOG_OBJECT_H_

