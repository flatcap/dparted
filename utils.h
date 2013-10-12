/* Copyright (c) 2013 Richard Russon (FlatCap)
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

#ifndef _UTILS_H_
#define _UTILS_H_

#include <map>
#include <string>
#include <vector>

#include "stringnum.h"

unsigned int execute_command2 (const std::string &command, std::string &input);
unsigned int execute_command (const std::string &command, std::vector<std::string> &output);
unsigned int execute_command (const std::string &command, std::string &output);
std::string get_size (long long size);
long long extract_number (const std::string &text, unsigned int &start);
std::string extract_quoted_string (const std::string &text, unsigned int &index);
std::string extract_bare_string (const std::string &text, unsigned int &index);
long extract_quoted_long (const std::string &text, unsigned int &index);
long long extract_quoted_long_long (const std::string &text, unsigned int &index);
int extract_dev_range (const std::string &text, std::string &device, int &start, int &finish, int element = 0);
unsigned int parse_tagged_line (const std::string &line, const char *separators, std::map<std::string,StringNum> &tags, bool clear_map = true);
unsigned int explode (const char *separators, const std::string &input, std::vector<std::string> &parts);
unsigned int explode_n (const char *separators, const std::string &input, std::vector<std::string> &parts, int max);
std::string read_file_line (const std::string &filename);
std::string read_uuid (unsigned char *buffer);
void dump_hex (unsigned char *buffer, int bufsize);
long long align (long long num, long long round);

#endif // _UTILS_H_

