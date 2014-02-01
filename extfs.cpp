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

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <cstring>

#include "extfs.h"
#include "utils.h"
#include "log_trace.h"

/**
 * ExtFs
 */
ExtFs::ExtFs (void)
{
}

/**
 * create
 */
ExtFsPtr
ExtFs::create (void)
{
	ExtFsPtr e (new ExtFs());
	e->weak = e;

	return e;
}


/**
 * parse_line
 */
static bool
parse_line (const std::string& line, std::string& key, std::string& value)
{
	// 23 chars, colon, 2+ spaces, value
	// ^[a-z #]+:  +[^ ].*$
	// find ":  "
	size_t pos;
	std::string k;
	std::string v;

	pos = line.find_first_of (':');
	if ((pos == std::string::npos) || (pos < 1))
		return false;

	k = line.substr (0, pos);
	v = line.substr (pos);

	pos = v.find_first_not_of (" 	", 1);	// Space, Tab
	if (pos == std::string::npos)
		return false;

	key   = k;
	value = v.substr (pos);
	return true;
}

/**
 * make_key
 */
std::string
make_key (std::string desc)
{
	// Lower case
	std::transform (desc.begin(), desc.end(), desc.begin(), ::tolower);

	// # -> number
	size_t pos = desc.find_first_of ('#');
	if (pos != std::string::npos) {
		desc.replace (pos, 1, "number");
	}

	// space -> underscore
	pos = -1;
	do {
		pos = desc.find_first_of (' ', pos+1);
		if (pos != std::string::npos) {
			desc[pos] = '_';
		}

	} while (pos != std::string::npos);

	return "extfs_" + desc;
}

/**
 * tune2fs
 */
std::map<std::string,std::string>
tune2fs (const std::string& dev)
{
	std::string command;
	std::vector<std::string> output;
	std::map<std::string,std::string> results;

	command = "tune2fs -l " + dev;
	execute_command1 (command, output);

	std::string key;
	std::string value;

	//printf ("keys:\n");
	for (auto line : output) {
		if (line.substr (0, 7) == "tune2fs")
			continue;

		if (!parse_line (line, key, value)) {
			std::cout << "Failed: " << line << std::endl;
			continue;
		}

		//printf ("\t>>%s<<\n", key.c_str());
		results[key] = value;
	}
	//printf ("\n");

	return results;
}


/**
 * get_ext_sb - super block
 */
void
ExtFs::get_ext_sb (ContainerPtr parent)
{
	//XXX return bool -- a quick match on the sb might not be enough -- tune2fs could fail
	if (!parent)
		return;

	std::string dev = parent->get_device_name();
	if (dev.empty())	//XXX shouldn't happen
		return;

	std::map<std::string,std::string> info = tune2fs (dev);
	if (info.empty()) {
		std::cout << "tune2fs failed" << std::endl;
		return;
	}

	std::map<std::string,std::string>::iterator it;

	it = info.find ("Filesystem volume name");
	if (it != std::end (info)) {
		name = (*it).second;
		if (name == "<none>") {
			name.clear();
		}
		info.erase (it);
	}

	it = info.find ("Filesystem UUID");
	if (it != std::end (info)) {
		uuid = (*it).second;
		info.erase (it);
	}

	it = info.find ("Block size");
	if (it != std::end (info)) {
		StringNum s ((*it).second);
		block_size = s;
		info.erase (it);
	}

	it = info.find ("Block count");
	if (it != std::end (info)) {
		StringNum s ((*it).second);
		bytes_size = s;
		bytes_size *= block_size;
		info.erase (it);
	}

	it = info.find ("Free blocks");
	if (it != std::end (info)) {
		StringNum s ((*it).second);
		bytes_used = s;
		bytes_used *= block_size;
		bytes_used = bytes_size - bytes_used;
		info.erase (it);
	}

#if 1
	// declare everything else
	const char* me = "ExtFs";
	more_props.reserve (info.size());	// if this vector is reallocated the app will die
	//printf ("Props:\n");
	for (auto i : info) {
		std::string desc  = i.first;
		std::string key   = make_key (desc);
		std::string value = i.second;
		//printf ("\t%-32s %-24s %s\n", key.c_str(), desc.c_str(), value.c_str());

		more_props.push_back (value);
		declare_prop (me, key.c_str(), more_props.back(), desc.c_str());
	}
#endif
}

/**
 * get_ext2
 */
ExtFsPtr
ExtFs::get_ext2 (ContainerPtr parent, unsigned char* buffer, int bufsize)
{
	bool b1 = (*(unsigned short int*) (buffer+0x438) == 0xEF53);	// Magic
	bool b2 = !(*(unsigned int*) (buffer + 0x45C) & 0x0000004);	// Journal

	ExtFsPtr e;
	if (b1 && b2) {
		e = create();
		e->sub_type ("ext2");
		e->get_ext_sb (parent);
		e->get_mounted_usage (parent);
	}
	return e;
}

/**
 * get_ext3
 */
ExtFsPtr
ExtFs::get_ext3 (ContainerPtr parent, unsigned char* buffer, int bufsize)
{
	bool b1 = (*(unsigned short int*) (buffer+0x438) == 0xEF53);	// Magic
	bool b2 = (*(unsigned int*) (buffer + 0x45C) & 0x0000004);	// Journal
	bool b3 = (*(unsigned int*) (buffer + 0x460) < 0x0000040);	// Small INCOMPAT
	bool b4 = (*(unsigned int*) (buffer + 0x464) < 0x0000008);	// Small RO_COMPAT

	ExtFsPtr e;
	if (b1 && b2 && b3 && b4) {
		e = create();
		e->sub_type ("ext3");
		e->get_ext_sb (parent);
		e->get_mounted_usage (parent);
	}
	return e;
}

/**
 * get_ext4
 */
ExtFsPtr
ExtFs::get_ext4 (ContainerPtr parent, unsigned char* buffer, int bufsize)
{
	bool b1 = (*(unsigned short int*) (buffer+0x438) == 0xEF53);	// Magic
	bool b2 = (*(unsigned int*) (buffer + 0x45C) & 0x0000004);	// Journal
	bool b3 = (*(unsigned int*) (buffer + 0x460) < 0x0000040);	// Small INCOMPAT
	bool b4 = (*(unsigned int*) (buffer + 0x460) > 0x000003f);	// Large INCOMPAT
	bool b5 = (*(unsigned int*) (buffer + 0x464) > 0x0000007);	// Large RO_COMPAT

	ExtFsPtr e;
	if (b1 && b2 && ((b3 && b5) || b4)) {
		e = create();
		e->sub_type ("ext4");
		e->get_ext_sb (parent);
		e->get_mounted_usage (parent);
	}
	return e;
}

