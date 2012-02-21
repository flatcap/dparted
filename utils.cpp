/* Copyright (c) 2012 Richard Russon (FlatCap)
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
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <parted/parted.h>
#include <glibmm/spawn.h>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "utils.h"
#include "stringnum.h"
#include "log.h"

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

	//log_debug ("text = >>%s<<\n", text.c_str());
	//log_debug ("index = %d, start = %d, finish = %d\n", index, start, finish);

	index = finish;

	return text.substr (start, finish - start);
}

/**
 * extract_bare_string
 */
std::string extract_bare_string (const std::string &text, unsigned int &index)
{
	unsigned int start  = -1;
	unsigned int finish = -1;

	start  = text.find_first_not_of (' ', index);
	finish = text.find_first_of ("\r\n", start);

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
	//log_debug ("\tindex = %d, colon = %d, dash = %d\n", index, colon, dash);

	device = text.substr (index, colon - index);

	start  = strtol (text.substr (colon + 1, dash - 1).c_str(), NULL, 10);
	finish = strtol (text.substr (dash + 1).c_str(), NULL, 10);

	//log_debug ("\tdevice = %s, start = %d, finish = %d\n", device.c_str(), start, finish);
	return 0;
}

/**
 * explode
 */
unsigned int explode (const char *separators, const std::string &input, std::vector<std::string> &parts)
{
	size_t start = 0;
	size_t end   = 0;

	if (!separators)
		return 0;

	parts.clear();

	end = input.find (separators, start);
	while (end != std::string::npos) {
		start = input.find_first_not_of (" \t", start);		// trim leading whitespace
		parts.push_back (input.substr (start, end - start));
		start = end + 1;
		end = input.find (separators, start);
	}

	if (start != input.size()) {
		parts.push_back (input.substr (start));
	}

#if 0
	log_debug ("vector:\n");
	std::vector<std::string>::iterator it2;
	for (it2 = parts.begin(); it2 != parts.end(); it2++) {

		std::string value = (*it2);

		log_debug ("\t>>%s<<\n", value.c_str());
	}
	log_debug ("\n");
#endif

	return parts.size();
}

/**
 * parse_tagged_line
 */
unsigned int parse_tagged_line (const std::string &line, const char *separators, std::map<std::string,StringNum> &tags, bool clear_map /* = true */)
{
	//XXX optional param: bool clear_array = true;
	int middle;
	std::vector<std::string> list;

	explode (separators, line, list);

	if (clear_map)
		tags.clear();

	std::vector<std::string>::iterator it;
	for (it = list.begin(); it != list.end(); it++) {
		middle = (*it).find ('=');				// XXX check it succeeds

		std::string name  = (*it).substr (0, middle);
		std::string value = (*it).substr (middle + 1);

		// trim quotes XXX
		unsigned int len = value.length();
		if (len > 0) {
			if ((value[0] == '"') || (value[0] == '\'')) {
				value = value.substr (1, len - 2);
			}
		}

		tags[name] = value;
	}

#if 0
	log_debug ("map:\n");
	std::map<std::string,StringNum>::iterator it2;
	for (it2 = tags.begin(); it2 != tags.end(); it2++) {

		std::string name  = (*it2).first;
		std::string value = (*it2).second;

		log_debug ("\t%s -> %s\n", name.c_str(), value.c_str());
	}
	log_debug ("\n");
#endif

	return tags.size();
}

/**
 * read_file_line
 */
std::string read_file_line (const std::string &filename)
{
	std::ifstream in (filename.c_str());

	std::string line ((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

	size_t pos = line.find_first_of ("\r\n");
	if (pos != std::string::npos)
		line = line.substr (0, pos);

	return line;
}

/**
 * read_uuid
 */
std::string read_uuid (unsigned char *buffer)
{
	char uuid[40];

	snprintf (uuid, sizeof (uuid), "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[9], buffer[10], buffer[11], buffer[12], buffer[13], buffer[14], buffer[15]);

	return uuid;
}

/**
 * dump_hex
 */
void dump_hex (unsigned char *buffer, int bufsize)
{
	int i;

	for (i = 0; i < bufsize; i += 16) {
		log_debug ("%06x  %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X %02X\n",
			i,
			buffer[i +  0], buffer[i +  1], buffer[i +  2], buffer[i +  3], buffer[i +  4], buffer[i +  5], buffer[i +  6], buffer[i +  7],
			buffer[i +  8], buffer[i +  9], buffer[i + 10], buffer[i + 11], buffer[i + 12], buffer[i + 13], buffer[i + 14], buffer[i + 15]);
	}
}

