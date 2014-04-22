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

#include <cstdio>
#include <cstring>

#include "log_object.h"
#include "log.h"
#include "utils.h"

LogObject::LogObject()
{
}

LogObject::~LogObject()
{
	if (file) {
		fclose (file);
		file = nullptr;
		// log_file ("closed: %s", filename.c_str());	//XXX reinstate when we can handle early/late logging
	}
}

bool
LogObject::open_file (const std::string& name, bool truncate /*=false*/)
{
	const char *flags;
	if (truncate) {
		flags = "ew+";		// create/truncate, close on exec
	} else {
		flags = "ae";		// append, close on exec
	}

	file = fopen (name.c_str(), flags);
	if (file) {
		filename = name;
		//log_file ("file open: %d, '%s'", fd, name.c_str());	//XXX reinstate when we can handle early/late logging

		if (reset_tty && (strncmp (filename.c_str(), "/dev/pts/", 9) == 0)) {
			fprintf (file, "\033c");
		}
	} else {
		filename.clear();
		log_error ("Failed to open '%s': %s", name.c_str(), strerror (errno));
	}

	return (file != nullptr);
}

void
LogObject::log_line (Severity UNUSED(level), const char* UNUSED(function), const char* UNUSED(file), int UNUSED(line), const char* message)
{
	if (!file) {
		log_error ("Log isn't initialised");
		return;
	}

	fprintf (file, "%s\n", message);
}

