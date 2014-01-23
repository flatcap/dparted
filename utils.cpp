/* Copyright (c) 2014 Richard Russon (FlatCap)
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

#include <cmath>
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

#include "log.h"
#include "stringnum.h"
#include "utils.h"
#include "log_trace.h"

/**
 * execute_command2
 */
unsigned int
execute_command2 (const std::string& command, std::string& input)
{
	FILE* file = nullptr;
	int count = 0;

	//XXX log command and output

	log_debug ("running command: %s\n", command.c_str());
	file = popen (command.c_str(), "we");
	if (file == nullptr) {
		log_error ("popen failed");	//XXX log_perror
		return -1;
	}

	count = fprintf (file, "%s\n", input.c_str());
	log_debug ("wrote %d bytes to command %s\n", count, command.c_str());

	if (pclose (file) == -1) {
		log_error ("pclose failed");	//XXX log_perror
		return -1;
	}

	return count;
}

/**
 * execute_command1
 */
unsigned int
execute_command1 (const std::string& command, std::vector<std::string>& output)
{
	FILE* file = nullptr;
	char* ptr = nullptr;
	size_t n = 0;
	int count = 0;

	output.clear();

	//XXX log command and output

	//log_debug ("running command: %s\n", command.c_str());
	// Execute command and save its output to stdout
	file = popen (command.c_str(), "r");
	if (file == nullptr) {
		//log_error ("popen failed");	//XXX log_perror
		return -1;
	}

	//log_debug ("output:\n");
	do {
		ptr = nullptr;
		n = 0;
		count = getline (&ptr, &n, file);		//XXX std::getline, istream::getline
		if (count > 0) {
			if (ptr[count-1] == '\n')
				ptr[count-1] = 0;
			output.push_back (ptr);
			//log_debug ("\t%s\n", ptr);
		}
		free (ptr);
	} while (count > 0);

	/*
	   XXX correct way to check return code
	int ret = pclose (fd);
	if (WIFEXITED(ret))
		  log_info ("%d\n", WEXITSTATUS(ret));
	*/
	int retcode = pclose (file);
	//log_info ("command %s returned %d\n", command.c_str(), retcode);
	if (retcode == -1) {
		//log_error ("pclose failed");	//XXX log_perror
		return -1;
	}

	return output.size();
}

/**
 * execute_command3
 */
unsigned int
execute_command3 (const std::string& command, std::string& output)
{
	std::vector<std::string> v;
	int count;

	count = execute_command1 (command, v);

	output.clear();
	for (auto it : v) {
		output += it;
	}

	return count;
}

/**
 * get_size
 */
std::string
get_size (long size)
{
	//XXX do this without log2?  use ffs
	char buffer[64];
	double power = log2 ((double) llabs (size)) + 0.5;
	const char* suffix = "";
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
	sprintf (buffer, "%0.3g%s", (double) size/divide, suffix);
	return buffer;
}

/**
 * extract_number
 */
long
extract_number (const std::string& text, unsigned int& index)
{
	const char* digits = "0123456789";
	unsigned int first = text.find_first_of     (digits, index);
	unsigned int last  = text.find_first_not_of (digits, first);
	std::string num;

	num = text.substr (first, last - first);

	index = last;
	return strtoll (num.c_str(), nullptr, 10);
}

/**
 * extract_quoted_string
 */
std::string
extract_quoted_string (const std::string& text, unsigned int& index)
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
std::string
extract_bare_string (const std::string& text, unsigned int& index)
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
long
extract_quoted_long (const std::string& text, unsigned int& index)
{
	unsigned int start  = -1;
	unsigned int finish = -1;
	std::string tmp;

	start  = text.find ('\'', index) + 1;
	finish = text.find ('\'', start);

	index = finish;

	tmp = text.substr (start, finish - start);
	return strtol (tmp.c_str(), nullptr, 10);
}

/**
 * extract_quoted_long_long
 */
long
extract_quoted_long_long (const std::string& text, unsigned int& index)
{
	unsigned int start  = -1;
	unsigned int finish = -1;
	std::string tmp;

	start  = text.find ('\'', index) + 1;
	finish = text.find ('\'', start);

	index = finish;

	tmp = text.substr (start, finish - start);
	return strtoll (tmp.c_str(), nullptr, 10);
}

/**
 * extract_dev_range
 */
int
extract_dev_range (const std::string& text, std::string& device, int& start, int& finish, int element /* = 0 */)
{
	int index = 0;

	for (int i = 0; i < element; i++) {
		index = text.find (' ', index) + 1;
	}

	int colon = text.find (':', index);
	int dash  = text.find ('-', index);
	//log_debug ("\tindex = %d, colon = %d, dash = %d\n", index, colon, dash);

	device = text.substr (index, colon - index);

	start  = strtol (text.substr (colon + 1, dash - 1).c_str(), nullptr, 10);
	finish = strtol (text.substr (dash + 1).c_str(), nullptr, 10);

	//log_debug ("\tdevice = %s, start = %d, finish = %d\n", device.c_str(), start, finish);
	return 0;
}

/**
 * explode
 */
unsigned int
explode (const char* separators, const std::string& input, std::vector<std::string>& parts)
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
	for (auto value : parts) {
		log_debug ("\t>>%s<<\n", value.c_str());
	}
	log_debug ("\n");
#endif

	return parts.size();
}

/**
 * explode
 */
unsigned int
explode_n (const char* separators, const std::string& input, std::vector<std::string>& parts, int max)
{
	size_t start = 0;
	size_t end   = 0;

	if (!separators)
		return 0;

	parts.clear();

	//log_info ("input      = '%s'\n", input.c_str());
	//log_info ("separators = '%s'\n", separators);

	start = input.find_first_not_of (separators, start);
	end   = input.find_first_of     (separators, start);
	//log_info ("start = %ld, end = %ld\n", start, end);

	while (end != std::string::npos) {
		parts.push_back (input.substr (start, end - start));

		start = input.find_first_not_of (separators, end+1);
		end   = input.find_first_of     (separators, start);
		//log_info ("start = %ld, end = %ld\n", start, end);

		max--;
		if (max < 2)
			break;
	}

	if (start != input.size()) {
		parts.push_back (input.substr (start));
	}

#if 0
	log_debug ("vector:\n");
	for (auto value : parts) {
		log_debug ("\t>>%s<<\n", value.c_str());
	}
	log_debug ("\n");
#endif

	return parts.size();
}

/**
 * parse_tagged_line
 */
unsigned int
parse_tagged_line (const std::string& line, const char* separators, std::map<std::string,StringNum>& tags, bool clear_map /* = true */)
{
	//XXX optional param: bool clear_array = true;
	int middle;
	std::vector<std::string> list;

	explode (separators, line, list);

	if (clear_map)
		tags.clear();

	for (auto it : list) {
		middle = it.find ('=');				// XXX check it succeeds

		std::string name  = it.substr (0, middle);
		std::string value = it.substr (middle + 1);

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
	for (auto it2 : tags) {

		std::string name  = it2.first;
		std::string value = it2.second;

		log_debug ("\t%s -> %s\n", name.c_str(), value.c_str());
	}
	log_debug ("\n");
#endif

	return tags.size();
}

/**
 * read_file_line
 */
std::string
read_file_line (const std::string& filename)
{
	std::ifstream in (filename.c_str());

	std::string line ((std::istreambuf_iterator<char> (in)), std::istreambuf_iterator<char>());

	size_t pos = line.find_first_of ("\r\n");
	if (pos != std::string::npos)
		line = line.substr (0, pos);

	return line;
}

/**
 * read_uuid1
 */
std::string
read_uuid1 (unsigned char* buffer)
{
	std::stringstream ss;

	ss << std::setfill ('0') << std::hex;

	for (int i = 0; i < 16; ++i) {
		if ((i == 4) || (i == 6) || (i == 8) || (i == 10))
			ss << "-";
		ss << std::setw(2) << static_cast<unsigned> (buffer[i]);
	}

	return ss.str();
}

/**
 * read_uuid2
 */
std::string
read_uuid2 (unsigned char* buffer)
{
	std::stringstream ss;

	ss << std::setfill ('0') << std::hex;

	for (int i = 0; i < 8; ++i) {
		ss << std::setw(2) << static_cast<unsigned> (buffer[i]);
	}

	return ss.str();
}

/**
 * read_uuid3
 */
std::string
read_uuid3 (unsigned char* buffer)
{
	std::stringstream ss;

	ss << std::setfill ('0') << std::hex;

	for (int i = 0; i < 4; ++i) {
		if (i == 2)
			ss << "-";
		ss << std::setw(2) << static_cast<unsigned> (buffer[i]);
	}

	return ss.str();
}


/**
 * dump_hex
 */
void
dump_hex (unsigned char* buffer, int bufsize)
{
	for (int i = 0; i < bufsize; i += 16) {
		log_debug ("%06x  %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X %02X\n",
			i,
			buffer[i +  0], buffer[i +  1], buffer[i +  2], buffer[i +  3], buffer[i +  4], buffer[i +  5], buffer[i +  6], buffer[i +  7],
			buffer[i +  8], buffer[i +  9], buffer[i + 10], buffer[i + 11], buffer[i + 12], buffer[i + 13], buffer[i + 14], buffer[i + 15]);
	}
}

/**
 * dump_hex2
 */
void
dump_hex2 (void* buf, int start, int length)
{
	int off, i, s, e;
	unsigned char* mem = (unsigned char*) buf;

#if 1
	unsigned char last[16];
	int same = 0;
#endif
	s =  start                & ~15;	// round down
	e = (start + length + 15) & ~15;	// round up

	for (off = s; off < e; off += 16) {
#if 1
		if (memcmp ((char*) buf+off, last, sizeof (last)) == 0) {
			if (!same) {
				log_info ("	        ...\n");
				same = 1;
			}
			if ((off + 16) < e)
				continue;
		} else {
			same = 0;
			memcpy (last, (char*) buf+off, sizeof (last));
		}
#endif

		if (off == s)
			log_info ("	%6.6x ", start);
		else
			log_info ("	%6.6x ", off);

		for (i = 0; i < 16; i++) {
			if (i == 8)
				log_info (" -");
			if (((off+i) >= start) && ((off+i) < (start+length)))
				log_info (" %02X", mem[off+i]);
			else
				log_info ("   ");
		}
		log_info ("  ");
		for (i = 0; i < 16; i++) {
			if (((off+i) < start) || ((off+i) >= (start+length)))
				log_info (" ");
			else if (isprint (mem[off + i]))
				log_info ("%c", mem[off + i]);
			else
				log_info (".");
		}
		log_info ("\n");
	}
}

/**
 * align
 */
long
align (long num, long round)
{
	if (round == 0)
		return num;
	return ((num + round - 1) / round * round);
}

