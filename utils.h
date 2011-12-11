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


#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>
#include <vector>
#include <map>

int execute_command (const std::string &command, std::string &output, std::string &error);
std::string get_size (long long size);
int iprintf (int indent, const char *format, ...) __attribute__((format(printf, 2, 3)));
long long extract_number (const std::string &text, unsigned int &start);
std::string extract_quoted_string (const std::string &text, unsigned int &index);
std::string extract_bare_string (const std::string &text, unsigned int &index);
long extract_quoted_long (const std::string &text, unsigned int &index);
long long extract_quoted_long_long (const std::string &text, unsigned int &index);
int extract_dev_range (const std::string &text, std::string &device, int &start, int &finish, int element = 0);
std::string get_fs (const std::string &device, long long offset);
unsigned int parse_tagged_line (const std::string &line, std::map<std::string,std::string> &tags);
unsigned int explode (const char *separators, const std::string &input, std::vector<std::string> &parts);

#endif // _UTILS_H_

