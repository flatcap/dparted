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

#ifndef _UTILS_H_
#define _UTILS_H_

#include <map>
#include <sstream>
#include <string>
#include <string>
#include <vector>

#include "stringnum.h"

#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#define WARN_UNUSED __attribute__((warn_unused_result))

template<class T>
void*
VP (const std::shared_ptr<T>& p)
{
	return (void*) p.get();
}

template<class T>
void*
VP (const std::weak_ptr<T>& w)
{
	std::shared_ptr<T> p = w.lock();
	return (void*) p.get();
}

const char* SP (const std::string&       str);
const char* SP (const std::stringstream& str);

std::uint64_t            align (std::uint64_t num, std::uint64_t round);
void                     delete_region (std::vector<std::pair<std::uint64_t, std::uint64_t>>& region, std::uint64_t start, std::uint64_t size);
std::string              demangle (const char* symbol);
void                     dump_hex2 (void* buf, std::uint64_t start, std::uint64_t length);
void                     dump_hex (std::uint8_t* buffer, std::uint64_t bufsize);
std::string              dump_regions (const std::string& desc, std::vector<std::pair<std::uint64_t, std::uint64_t>>& region);
int                      execute_command_in  (const std::string& command, const std::string& input,         bool log_input  = true, bool log_cmd = true);
int                      execute_command_out (const std::string& command, std::vector<std::string>& output, bool log_output = true, bool log_cmd = true);
unsigned int             explode (const char* separators, const std::string& input, std::vector<std::string>& parts, int max = 999);
std::vector<std::string> get_backtrace (bool reverse = false);
std::string              get_fixed_str (const void *buffer, std::uint32_t maxlen);
std::string              get_size (std::uint64_t size);
std::string              join (std::vector<std::string> v, const std::string& sep);
std::string              make_part_dev (std::string device, int number);
unsigned int             parse_tagged_line (const std::string& line, const char* separators, std::map<std::string, StringNum>& tags, bool clear_map = true);
std::string              read_file_line (const std::string& filename);
std::string              read_uuid1 (std::uint8_t* buffer);
std::string              read_uuid2 (std::uint8_t* buffer);
std::string              read_uuid3 (std::uint8_t* buffer);
std::string              shorten_device (const std::string& device);

#endif // _UTILS_H_

