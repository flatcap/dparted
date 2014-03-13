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
#include <string>
#include <vector>

#include "stringnum.h"

unsigned int execute_command2 (const std::string& command, std::string& input);
unsigned int execute_command1 (const std::string& command, std::vector<std::string>& output);
unsigned int execute_command3 (const std::string& command, std::string& output);
std::string get_size (long size);
unsigned int parse_tagged_line (const std::string& line, const char* separators, std::map<std::string,StringNum>& tags, bool clear_map = true);
unsigned int explode (const char* separators, const std::string& input, std::vector<std::string>& parts);
unsigned int explode_n (const char* separators, const std::string& input, std::vector<std::string>& parts, int max);
std::string read_file_line (const std::string& filename);
std::string read_uuid1 (unsigned char* buffer);
std::string read_uuid2 (unsigned char* buffer);
std::string read_uuid3 (unsigned char* buffer);
void dump_hex (unsigned char* buffer, int bufsize);
void dump_hex2 (void* buf, int start, int length);
long align (long num, long round);

#endif // _UTILS_H_

