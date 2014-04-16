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

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <cxxabi.h>
#include <execinfo.h>

#include "log.h"
#include "stringnum.h"
#include "utils.h"

std::uint64_t
align (std::uint64_t num, std::uint64_t round)
{
	return_val_if_fail (round, num);

	return ((num + round - 1) / round * round);
}

void
delete_region (std::vector<std::pair<std::uint64_t,std::uint64_t>>& region, std::uint64_t start, std::uint64_t size)
{
	std::uint64_t finish = start+size-1;

	for (auto it = std::begin (region); it != std::end (region); ++it) {
		if (finish < (*it).first)
			continue;

		if (start > (*it).second)
			continue;

		/* xxxx = Delete, <--> = Remainder
		 * 1 |xxxxxxxxxxx|	 s &  f		delete entire region
		 * 2 |xxx<------>|	 s & !f		move start
		 * 3 |<------>xxx|	!s &  f		move end
		 * 4 |<-->xxx<-->|	!s & !f		split region
		 */
		if (start == (*it).first) {
			if (finish == (*it).second) {		//1
				region.erase (it);
				break;
			} else {				//2
				(*it).first = finish+1;
				break;
			}
		} else {
			if (finish == (*it).second) {		//3
				(*it).second = start-1;
				break;
			} else {				//4
				int end = (*it).second;
				(*it).second = start-1;
				region.insert (it+1, { finish+1, end });
				break;
			}
		}
	}
}

std::string
demangle (const char* symbol)
{
	std::vector<char> buffer;
	buffer.resize (256);	// Most prototypes should be shorter than this

	char* demangled = nullptr;

	// C++ stack: demangle2(_Z7mytracev+0x20) [0x4019f1]
	if (sscanf (symbol, "%*[^(]%*[^_]%255[^)+]", buffer.data()) == 1) {
		if ((demangled = abi::__cxa_demangle (buffer.data(), NULL, NULL, NULL))) {
			std::string result (demangled);
			free (demangled);
			return result;
		}
	}

	// Typeinfo: 3barI5emptyLi17EE
	if ((demangled = abi::__cxa_demangle (symbol, NULL, NULL, NULL))) {
		std::string result (demangled);
		free (demangled);
		return result;
	}

	// C stack: demangle2(main+0x19) [0x401bc8]
	if (sscanf (symbol, "%*[^(](%255[^)+]", buffer.data()) == 1) {
		if (strncmp (buffer.data(), "main", 4) == 0) {
			return "main(int, char**)";
		} else {
			return buffer.data();
		}
	}

	return symbol;
}

void
dump_hex (std::uint8_t* buffer, std::uint64_t bufsize)
{
	for (std::uint64_t i = 0; i < bufsize; i += 16) {
		log_hex ("%06lx  %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X %02X",
			i,
			buffer[i +  0], buffer[i +  1], buffer[i +  2], buffer[i +  3], buffer[i +  4], buffer[i +  5], buffer[i +  6], buffer[i +  7],
			buffer[i +  8], buffer[i +  9], buffer[i + 10], buffer[i + 11], buffer[i + 12], buffer[i + 13], buffer[i + 14], buffer[i + 15]);
	}
}

void
dump_hex2 (void* buf, std::uint64_t start, std::uint64_t length)
{
	std::uint64_t off, i, s, e;
	std::uint8_t* mem = (std::uint8_t*) buf;

	std::vector<char> last (16, 0);
	bool same = false;

	s =  start                & ~15;	// round down
	e = (start + length + 15) & ~15;	// round up

	for (off = s; off < e; off += 16) {
		if ((memcmp ((char*) buf+off, last.data(), last.size()) == 0) && ((off + 16) < e) && (off > s)) {
			if (!same) {
				log_hex ("          ...");
				same = true;
			}
			continue;
		} else {
			same = false;
			memcpy (last.data(), (char*) buf+off, last.size());
		}

		std::stringstream ss;
		if (off == s) {
			ss << std::uppercase << std::setfill('0') << std::setw(8) << std::hex << start << " ";
		} else {
			ss << std::uppercase << std::setfill('0') << std::setw(8) << std::hex << off << " ";
		}

		for (i = 0; i < 16; ++i) {
			if (i == 8) {
				ss << " -";
			}
			if (((off+i) >= start) && ((off+i) < (start+length))) {
				ss << " " << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (int) mem[off+i];
			} else {
				ss << "   ";
			}
		}
		ss << "  ";
		for (i = 0; i < 16; ++i) {
			if (((off+i) < start) || ((off+i) >= (start+length))) {
				ss << " ";
			} else if (isprint (mem[off + i])) {
				ss << (char) (mem[off + i]);
			} else {
				ss << ".";
			}
		}
		log_hex (ss);
	}
}

void
dump_regions (const std::string& desc, std::vector<std::pair<std::uint64_t,std::uint64_t>>& region)
{
	log_debug (desc);
	std::stringstream ss;
	ss << '\t';
	if (region.empty()) {
		ss << "empty";
	}
	for (auto r : region) {
		ss << r.first << "-" << r.second;
	}
	log_debug (ss);
}

/**
 * execute_command_out - output (vector<string>)
 */
unsigned int
execute_command_out (const std::string& command, std::vector<std::string>& output, bool log_output /*=true*/, bool log_command /*=true*/)
{
	FILE* file = nullptr;
	char* ptr = nullptr;
	std::size_t n = 0;
	int count = 0;

	if (log_command) {
		log_command ("running command: '%s'", command.c_str());
	}

	// Execute command and save its output to stdout
	file = popen (command.c_str(), "r");
	if (file == nullptr) {
		log_error ("popen failed: %s", strerror (errno));
		return -1;
	}

	output.clear();

	if (log_output) {
		log_command_out ("output:");
	}
	do {
		ptr = nullptr;
		n = 0;
		count = getline (&ptr, &n, file);		//XXX std::getline, istream::getline
		if (count > 0) {
			if (ptr[count-1] == '\n') {
				ptr[count-1] = 0;
			}
			output.push_back (ptr);
			if (log_output) {
				log_command_out ("\t%s", ptr);
			}
		}
		free (ptr);
	} while (count > 0);

	int retcode = pclose (file);
	if (!WIFEXITED (retcode)) {
		log_error ("Command exited unexpectedly");
		return -1;
	}

	if (log_command) {
		log_command ("command returned %d", retcode);
	}

	return WEXITSTATUS (retcode);
}

/**
 * execute_command_in - input (string)
 */
unsigned int
execute_command_in  (const std::string& command, std::string& input, bool log_input /*=true*/, bool log_command /*=true*/)
{
	FILE* file = nullptr;

	if (log_command) {
		log_command ("running command: '%s'", command.c_str());
	}

	// Execute command and write our stdout to its input
	file = popen (command.c_str(), "we");
	if (file == nullptr) {
		log_error ("popen failed: %s", strerror (errno));
		return -1;
	}

	int count = fprintf (file, "%s\n", input.c_str());
	if (log_input) {
		log_command_in ("wrote %d bytes to command", count);
		log_command_in ("%s", input.c_str());
	}

	int retcode = pclose (file);
	if (!WIFEXITED (retcode)) {
		log_error ("Command exited unexpectedly");
		return -1;
	}

	if (log_command) {
		log_command ("command returned %d", retcode);
	}

	return WEXITSTATUS (retcode);
}

unsigned int
explode (const char* separators, const std::string& input, std::vector<std::string>& parts)
{
	return_val_if_fail (separators, 0);

	std::size_t start = 0;
	std::size_t end   = 0;

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

	log_debug ("vector:");
	for (auto value : parts) {
		log_debug ("\t>>%s<<", value.c_str());
	}

	return parts.size();
}

unsigned int
explode_n (const char* separators, const std::string& input, std::vector<std::string>& parts, int max)
{
	return_val_if_fail (separators, 0);

	std::size_t start = 0;
	std::size_t end   = 0;

	parts.clear();

	log_info ("input      = '%s'", input.c_str());
	log_info ("separators = '%s'", separators);

	start = input.find_first_not_of (separators, start);
	end   = input.find_first_of     (separators, start);
	log_info ("start = %ld, end = %ld", start, end);

	while (end != std::string::npos) {
		parts.push_back (input.substr (start, end - start));

		start = input.find_first_not_of (separators, end+1);
		end   = input.find_first_of     (separators, start);
		log_info ("start = %ld, end = %ld", start, end);

		max--;
		if (max < 2)
			break;
	}

	if (start != input.size()) {
		parts.push_back (input.substr (start));
	}

	log_debug ("vector:");
	for (auto value : parts) {
		log_debug ("\t>>%s<<", value.c_str());
	}

	return parts.size();
}

std::vector<std::string>
get_backtrace (bool reverse /*=false*/)
{
	const int MAX_FRAMES = 100;
	std::vector<std::string> bt;

	void* addresses[MAX_FRAMES];
	int size = backtrace (addresses, MAX_FRAMES);
	char** symbols = backtrace_symbols (addresses, size);
	for (int x = 1; x < size; ++x) {			// Skip 0 (ourself)
		std::string sym = demangle (symbols[x]);
		bt.push_back (sym);
		if (sym.substr (0, 4) == "main")		// Just libc after this
			break;
	}
	free (symbols);
	if (reverse) {
		std::reverse (std::begin (bt), std::end (bt));
	}
	return bt;
}

std::string
get_fixed_str (const void *buffer, std::uint32_t maxlen)
{
	return_val_if_fail (buffer, {});
	return_val_if_fail (maxlen, {});

	int len = strnlen ((const char*) buffer, maxlen);
	return std::string ((const char*) buffer, len);
}

std::string
get_size (std::uint64_t size)
{
	//XXX do this without log2?  use __builtin_clz
	std::stringstream ss;
	double power = log2 ((double) llabs (size)) + 0.5;
	const char* suffix = "";
	double divide = 1;

	     if (power < 10) { suffix =   " B"; divide =                1; }
	else if (power < 20) { suffix = " KiB"; divide =             1024; }
	else if (power < 30) { suffix = " MiB"; divide =          1048576; }
	else if (power < 40) { suffix = " GiB"; divide =       1073741824; }
	else if (power < 50) { suffix = " TiB"; divide =    1099511627776; }
	else if (power < 60) { suffix = " PiB"; divide = 1125899906842624; }

	ss << std::setprecision(3) << (double) size/divide << suffix;
	return ss.str();
}

std::string
join (std::vector<std::string> v, const std::string& sep)
{
	std::string j;

	if (v.empty())
		return j;

	auto it  = std::begin(v);
	auto end = std::end(v);

	j = *it;
	++it;

	for (; it != end; ++it) {
		j += sep + *it;
	}

	return j;
}

std::string
make_part_dev (std::string device, int number)
{
	if (device.empty())
		return {};

	if (number < 1)
		return {};

	if (isdigit (device.back())) {
		device += 'p';
	}
	device += std::to_string (number);

	return device;
}

unsigned int
parse_tagged_line (const std::string& line, const char* separators, std::map<std::string,StringNum>& tags, bool clear_map /* = true */)
{
	int middle;
	std::vector<std::string> list;

	explode (separators, line, list);

	if (clear_map)
		tags.clear();

	for (auto it : list) {
		middle = it.find ('=');				//XXX check it succeeds

		std::string name  = it.substr (0, middle);
		std::string value = it.substr (middle + 1);

		//XXX trim quotes
		unsigned int len = value.length();
		if (len > 0) {
			if ((value[0] == '"') || (value[0] == '\'')) {
				value = value.substr (1, len - 2);
			}
		}

		tags[name] = value;
	}

	log_debug ("map:");
	for (auto it2 : tags) {

		std::string name  = it2.first;
		std::string value = it2.second;

		log_debug ("\t%s -> %s", name.c_str(), value.c_str());
	}

	return tags.size();
}

std::string
read_file_line (const std::string& filename)
{
	std::ifstream in (filename.c_str());

	std::string line ((std::istreambuf_iterator<char> (in)), std::istreambuf_iterator<char>());

	std::size_t pos = line.find_first_of ("\r\n");
	if (pos != std::string::npos)
		line = line.substr (0, pos);

	return line;
}

std::string
read_uuid1 (std::uint8_t* buffer)
{
	std::stringstream ss;

	ss << std::setfill ('0') << std::hex;

	for (int i = 0; i < 16; ++i) {
		if ((i == 4) || (i == 6) || (i == 8) || (i == 10)) {
			ss << "-";
		}
		ss << std::setw(2) << static_cast<unsigned> (buffer[i]);
	}

	return ss.str();
}

std::string
read_uuid2 (std::uint8_t* buffer)
{
	std::stringstream ss;

	ss << std::setfill ('0') << std::hex;

	for (int i = 0; i < 8; ++i) {
		ss << std::setw(2) << static_cast<unsigned> (buffer[i]);
	}

	return ss.str();
}

std::string
read_uuid3 (std::uint8_t* buffer)
{
	std::stringstream ss;

	ss << std::setfill ('0') << std::hex;

	for (int i = 0; i < 4; ++i) {
		if (i == 2) {
			ss << "-";
		}
		ss << std::setw(2) << static_cast<unsigned> (buffer[i]);
	}

	return ss.str();
}

std::string
shorten_device (const std::string& device)
{
	std::size_t pos = device.find ("/dev/");
	if (pos == std::string::npos)
		return device;

	if (pos != 0)
		return device;

	return device.substr(5);
}


