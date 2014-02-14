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

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <cstring>

#include "btrfs.h"
#include "utils.h"

/**
 * Btrfs
 */
Btrfs::Btrfs (void)
{
}

/**
 * create
 */
BtrfsPtr
Btrfs::create (void)
{
	BtrfsPtr b (new Btrfs());
	b->weak = b;

	return b;
}


/**
 * get_btrfs_usage
 */
bool
Btrfs::get_btrfs_usage (void)
{
	if (device.empty())
		return false;
	ContainerPtr c = get_smart();
	if (get_mounted_usage(c))
		return true;

	return false;
}

/**
 * parse_header
 */
bool
parse_header (const std::string& header, std::string& dev)
{
	// "superblock: bytenr=65536, device=/dev/loop1p1"

	if (header.substr (0, 19) != "superblock: bytenr=")
		return false;

	size_t pos = header.find ("device=");
	if (pos == std::string::npos)
		return false;

	dev = header.substr (pos+7);
	return true;
}

/**
 * parse_line
 */
static bool
parse_line (const std::string& line, std::string& key, std::string& value)
{
	// Up to 21 chars, tab(s), value
	size_t pos;
	std::string k;
	std::string v;

	pos = line.find_first_of ('\t');
	if ((pos == std::string::npos) || (pos < 1))
		return false;

	k = line.substr (0, pos);
	v = line.substr (pos);

	pos = v.find_first_not_of ('\t', 1);
	if (pos != std::string::npos) {
		v = v.substr (pos);
	}

	key   = k;
	value = v;
	return true;
}

/**
 * make_desc
 */
static std::string
make_desc (std::string key)
{
	// underscore/dot -> space
	size_t pos = -1;
	do {
		pos = key.find_first_of ("_.", pos+1);
		if (pos != std::string::npos) {
			key[pos] = ' ';
		}

	} while (pos != std::string::npos);

	key[0] = ::toupper (key[0]);
	return key;
}

/**
 * btrfs_show_super
 */
std::map<std::string,std::string>
btrfs_show_super (const std::string& dev)
{
	std::string command;
	std::vector<std::string> output;
	std::map<std::string,std::string> results;

	command = "btrfs-show-super " + dev;
	execute_command1 (command, output);

	std::string key;
	std::string value;

	//printf ("keys:\n");
	for (auto line : output) {
		//XXX move these two tests before loop and doctor vector
		if (line.substr (0, 11) == "superblock:") {
			std::string dev2;
			if (!parse_header (line, dev2)) {
				std::cout << "btrfs: bad header" << std::endl;
				break;
			}
			if (dev != dev2) {
				std::cout << "btrfs: devices don't match" << std::endl;
				break;
			}
			continue;
		} else if (line.substr (0, 10) == "----------") {
			continue;
		} else if (line.empty()) {
			continue;
		} else if (!parse_line (line, key, value)) {
			std::cout << "btrfs failed: " << line << std::endl;
			continue;
		}

		//printf ("\t>>%s<<\n", key.c_str());
		results[key] = value;
	}
	//printf ("\n");

	return results;
}

/**
 * get_btrfs_sb
 */
void
Btrfs::get_btrfs_sb (ContainerPtr parent)
{
	//XXX return bool -- a quick match on the sb might not be enough -- tune2fs could fail
	if (!parent)
		return;

	std::string dev = parent->get_device_name();
	if (dev.empty())	//XXX shouldn't happen
		return;

	std::map<std::string,std::string> info = btrfs_show_super (dev);
	if (info.empty()) {
		std::cout << "btrfs_show_super failed" << std::endl;
		return;
	}

	// declare everything else
	const char* me = "Btrfs";
	more_props.reserve (info.size());	// if this vector is reallocated the app will die
	//printf ("Props:\n");
	for (auto i : info) {
		std::string key   = i.first;
		std::string desc  = make_desc (key);
		std::string value = i.second;
		//printf ("\t%-32s %-24s %s\n", key.c_str(), desc.c_str(), value.c_str());

		more_props.push_back (value);
		declare_prop (me, key.c_str(), more_props.back(), desc.c_str());
	}
}

/**
 * get_btrfs
 */
BtrfsPtr
Btrfs::get_btrfs (ContainerPtr parent, unsigned char* buffer, int bufsize)
{
	if (strncmp ((char*) buffer+65600, "_BHRfS_M", 8) != 0)
		return nullptr;

	BtrfsPtr b  = Btrfs::create();
	b->sub_type ("btrfs");

	b->name = (char*) (buffer+0x1012B);
	b->uuid = read_uuid1 (buffer + 0x10020);

	b->bytes_size = *(long*) (buffer + 0x10070);	// 64 bits
	b->bytes_used = *(long*) (buffer + 0x10078);	// 64 bits
	b->block_size = *(int*)  (buffer + 0x10090);	// 32 bits

	b->get_btrfs_sb (parent);
	b->get_btrfs_usage();
	return b;
}


/**
 * get_actions
 */
std::vector<Action>
Btrfs::get_actions (void)
{
	// LOG_TRACE;
	std::vector<Action> actions = {
		//{ "create.filesystem", true },
	};

	std::vector<Action> parent_actions = Filesystem::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

/**
 * perform_action
 */
bool
Btrfs::perform_action (Action action)
{
	if (action.name == "create.table") {
		std::cout << "Btrfs perform: " << action.name << std::endl;
		return true;
	} else {
		std::cout << "Unknown action: " << action.name << std::endl;
		return false;
	}
}


