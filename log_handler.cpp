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
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

#include <sys/time.h>
#include <unistd.h>

#include "log_handler.h"
#include "log.h"
#include "utils.h"

LogHandler::LogHandler (void)
{
}

LogHandler::~LogHandler()
{
	stop();
}

LogHandlerPtr
LogHandler::create (const std::string& filename, bool truncate /*=false*/)
{
	const char *flags;
	if (truncate) {
		flags = "we";		// create/truncate, close on exec
	} else {
		flags = "ae";		// append, close on exec
	}

	FILE* f = fopen (filename.c_str(), flags);
	if (!f) {
		log_error ("Failed to open '%s': %s", filename.c_str(), strerror (errno));
		return nullptr;
	}

	//log_file ("file open: '%s'", name.c_str());	//XXX reinstate when we can handle early/late logging

	LogHandlerPtr lh (new LogHandler());
	lh->file       = f;
	lh->close_file = true;		// We opened it
	lh->filename   = filename;

	lh->is_tty = (strncmp (filename.c_str(), "/dev/pts/", 9) == 0);
	if (truncate) {
		lh->reset();
	}

	return lh;
}

LogHandlerPtr
LogHandler::create (FILE* handle)
{
	LogHandlerPtr lh (new LogHandler());
	lh->file = handle;

	return lh;
}


void
LogHandler::start (Severity level)
{
	auto cb = std::bind (&LogHandler::log_line, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6);
	log_handle = log_add_handler (cb, level);
}

void
LogHandler::stop (void)
{
	if (log_handle >= 0) {
		log_remove_handler (log_handle);
		log_handle = -1;
	}

	if (close_file) {
		fclose (file);
		log_file ("file close: '%s'", filename.c_str());
		file = nullptr;
		close_file = false;
	}
}

void
LogHandler::log_line (std::uint64_t index, Severity level, const char* function, const char* filename, int line, const char* message)
{
	return_if_fail (file);

	std::string str;
	std::string end;
	bool colour = ((foreground >= 0) || (background >= 0) || bold || underline);

	if (colour) {
		str += "\033[";
		if (underline) {
			str += "04;";
		}
		if (bold) {
			str += "01;";
		}
		if (foreground >= 0) {
			str += "38;5;" + std::to_string (foreground) + ";";
		}
		if (background >= 0) {
			str += "48;5;" + std::to_string (background) + ";";
		}
		str.back() = 'm';	// replace trailing semi-colon with 'm'
	}

	if (uniq_index) {
		std::stringstream ss;
		ss << std::setfill('0') << std::setw(6) << index;
		str += ss.str() + " ";
	}

	if (timestamp) {
		std::vector<char> tstr (20, 0);
		std::time_t now = std::time (nullptr);
		if (std::strftime(tstr.data(), tstr.size(), "%F %T", std::localtime (&now)) != 0) {
			str += tstr.data();

			if (microseconds) {
				struct timeval tv;
				gettimeofday(&tv, nullptr);
				std::stringstream ss;
				ss << std::setfill('0') << std::setw(6) << tv.tv_usec;
				str += "." + ss.str();
			}
			str += " ";
		}
	}

	if (show_level) {
		str += "[";
		str += log_get_level_name (level);
		str += "] ";
	}

	if (show_file_line) {
		str += filename;
		str += ":";
		str += line;
		str += " ";
	}

	if (show_function) {
		str += function;
		str += " ";
	}

	if (colour) {
		end = "\033[0m";
	}

	fprintf (file, "%s%s%s\n", str.c_str(), message, end.c_str());

	if (flush_line) {
		flush();
	}
}

void
LogHandler::flush (void)
{
	return_if_fail (file);
	fflush (file);
}

void
LogHandler::reset (void)
{
	return_if_fail (file);
	if (is_tty) {
		fprintf (file, "\033c");		// Reset the terminal
	}
}

