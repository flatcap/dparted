/* Copyright (c) 2011 Richard Russon (FlatCap)
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Library General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#include <stdio.h>
#include <glibmm/spawn.h>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <string>

#include "utils.h"

/**
 * execute_command
 */
int execute_command (const std::string &command, std::string &output, std::string &error)
{
	int exit_status = -1;
	std::string std_out;
	std::string std_error;

	try
	{
		std::vector<std::string>argv;
		argv.push_back ("sh");
		argv.push_back ("-c");
		argv.push_back (command);
		//argv.push_back ("sudo " + command);

		//Spawn command inheriting the parent's environment
		Glib::spawn_sync (".", argv, Glib::SPAWN_SEARCH_PATH, sigc::slot<void>(), &std_out, &std_error, &exit_status);
	}
	catch  (Glib::Exception & e)
	{
		 error = e.what();

		 return -1;
	}

	output = std_out;
	error = std_error;

	return exit_status;
}

/**
 * get_size
 */
std::string get_size (long long size)
{
	char buffer[64];
	double power = log2 ((double) llabs (size)) + 0.5;
	const char *suffix = "";
	double divide = 1;

	if (power < 10) {
		suffix = "   B";
		divide = 1;
	} else if (power < 20) {
		suffix = " KiB";
		divide = 1024;
	} else if (power < 30) {
		suffix = " MiB";
		divide = 1048576;
	} else if (power < 40) {
		suffix = " GiB";
		divide = 1073741824;
	} else if (power < 50) {
		suffix = " TiB";
		divide = 1099511627776;
	} else if (power < 60) {
		suffix = " PiB";
		divide = 1125899906842624;
	}
	sprintf (buffer, "%0.3g%s", (double)size/divide, suffix);
	return buffer;
}

/**
 * iprintf - indented printf
 */
__attribute__((format(printf, 2, 3)))
int iprintf(int indent, const char *format, ...)
{
	const char *space = "                                                                ";
	va_list ap;
	int result = 0;

	result = printf ("%.*s", indent, space);
	va_start(ap, format);
	result += vfprintf(stdout, format, ap);
	va_end(ap);
	return result;
}

/**
 * extract_number
 */
long long extract_number (const std::string &text, unsigned int &index)
{
	const char *digits = "0123456789";
	unsigned int first = text.find_first_of     (digits, index);
	unsigned int last  = text.find_first_not_of (digits, first);
	std::string num;

	num = text.substr (first, last - first);

	index = last;
	return strtoll (num.c_str(), NULL, 10);
}

/**
 * extract_quoted_string
 */
std::string extract_quoted_string (const std::string &text, unsigned int &index)
{
	unsigned int start  = -1;
	unsigned int finish = -1;

	start  = text.find ('\'', index) + 1;
	finish = text.find ('\'', start);

	index = finish;

	return text.substr (start, finish - start);
}

/**
 * extract_quoted_long
 */
long extract_quoted_long (const std::string &text, unsigned int &index)
{
	unsigned int start  = -1;
	unsigned int finish = -1;
	std::string tmp;

	start  = text.find ('\'', index) + 1;
	finish = text.find ('\'', start);

	index = finish;

	tmp = text.substr (start, finish - start);
	return strtol (tmp.c_str(), NULL, 10);
}

/**
 * extract_quoted_long_long
 */
long long extract_quoted_long_long (const std::string &text, unsigned int &index)
{
	unsigned int start  = -1;
	unsigned int finish = -1;
	std::string tmp;

	start  = text.find ('\'', index) + 1;
	finish = text.find ('\'', start);

	index = finish;

	tmp = text.substr (start, finish - start);
	return strtoll (tmp.c_str(), NULL, 10);
}

/**
 * extract_dev_range
 */
int extract_dev_range (const std::string &text, std::string &device, int &start, int &finish, int element /* = 0 */)
{
	int index = 0;

	for (int i = 0; i < element; i++) {
		index = text.find (' ', index) + 1;
	}

	int colon = text.find (':', index);
	int dash  = text.find ('-', index);
	//printf ("\tindex = %d, colon = %d, dash = %d\n", index, colon, dash);

	device = text.substr (index, colon - index);

	start  = strtol (text.substr (colon + 1, dash - 1).c_str(), NULL, 10);
	finish = strtol (text.substr (dash + 1).c_str(), NULL, 10);

	//printf ("\tdevice = %s, start = %d, finish = %d\n", device.c_str(), start, finish);
	return 0;
}

/**
 * get_lines
 */
unsigned int get_lines (const std::string &output, std::vector<std::string> &lines)
{
	int start = 0;
	int end   = -1;

	while ((end = output.find ('\n', start)) > 0) {
		std::string tmp = output.substr (start, end - start);
		lines.push_back (tmp);
		start = end + 1;
	}
	return lines.size();
}
