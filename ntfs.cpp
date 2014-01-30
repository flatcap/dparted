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

#include "ntfs.h"
#include "utils.h"
#include "log_trace.h"

/**
 * Ntfs
 */
Ntfs::Ntfs (void)
{
}

/**
 * create
 */
NtfsPtr
Ntfs::create (void)
{
	NtfsPtr n (new Ntfs());
	n->weak = n;

	return n;
}


/**
 * get_ntfs_usage
 */
bool
Ntfs::get_ntfs_usage (void)
{
	if (device.empty())
		return false;
	ContainerPtr c = get_smart();
	if (get_mounted_usage(c))
		return true;

	return false;
}

/**
 * parse_line
 */
static bool
parse_line (std::string line, std::string& key, std::string& value)
{
	// Tab, description, colon, space, value
	size_t pos;
	std::string k;
	std::string v;

	if (line.empty())
		return false;

	if (line[0] != '\t')
		return false;

	if (line.substr (0, 20) == "\tAllocated clusters ") {
		//XXX cheat a bit
		line.replace (19, 1, ": ");
	}

	pos = line.find_first_of (':');
	if (pos == std::string::npos)
		return false;

	k = line.substr (1, pos-1);
	v = line.substr (pos+1);

	if (v.empty())
		return false;

	if (v[0] != ' ')
		return false;

	v = v.substr (1);

	key   = k;
	value = v;
	return true;
}

/**
 * make_key
 */
static std::string
make_key (std::string desc)
{
	// Lower case
	std::transform (desc.begin(), desc.end(), desc.begin(), ::tolower);

	// space -> underscore
	size_t pos = -1;
	do {
		pos = desc.find_first_of (' ', pos+1);
		if (pos != std::string::npos) {
			desc[pos] = '_';
		}

	} while (pos != std::string::npos);

	return "ntfs_" + desc;
}


/**
 * get_ntfs_sb
 */
void
Ntfs::get_ntfs_sb (ContainerPtr parent)
{
	//XXX return bool -- a quick match on the sb might not be enough -- tune2fs could fail
	if (!parent)
		return;

	std::string dev = parent->get_device_name();
	if (dev.empty())	//XXX shouldn't happen
		return;

	std::string command;
	std::vector<std::string> output;

	command = "ntfsinfo -m " + dev;
	execute_command1 (command, output);	//XXX return value?

	more_props.reserve (output.size());	// if this vector is reallocated the app will die

	std::string key;
	std::string value;
	std::string desc;
	std::string section;

	long clust = -1;	// cluster size
	long csize = -1;	// total size in clusters
	long cfree = -1;	// free space in clusters

	//printf ("keys:\n");
	for (auto line : output) {
		if (line.empty())
			continue;

		if (line[0] != '\t') {
			section = "Ntfs " + line;
			//XXX trim trailing whitespace
			continue;
		}

		if (!parse_line (line, desc, value)) {
			std::cout << "ntfs failed: " << line << std::endl;
			continue;
		}

		//XXX clumsy
		StringNum s (value);
		     if (desc == "Volume Name")			name  = value;
		else if (desc == "Cluster Size")		clust = s;
		else if (desc == "Volume Size in Clusters")	csize = s;
		else if (desc == "Free Clusters")		cfree = s;

		key = make_key (desc);

		more_props.push_back (value);
		declare_prop (section.c_str(), key.c_str(), more_props.back(), desc.c_str());
	}

	//printf ("%ld, %ld, %ld\n", clust, csize, cfree);
	if ((clust > 0) && (csize > 0) && (cfree > 0)) {
		block_size = clust;
		bytes_size = csize * clust;
		bytes_used = bytes_size - (cfree * clust);
	} else {
		//XXX failed to get vital info
	}
}

/**
 * get_ntfs
 */
NtfsPtr
Ntfs::get_ntfs (ContainerPtr parent, unsigned char* buffer, int bufsize)
{
	if (strncmp ((char*) buffer+3, "NTFS    ", 8) != 0)
		return nullptr;

	std::string uuid = read_uuid2 (buffer + 72);
	long size = *(long*) (buffer + 40) * 512;

	NtfsPtr n = Ntfs::create();
	n->sub_type ("ntfs");

	n->uuid = uuid;
	n->bytes_size = size;

	n->get_ntfs_sb (parent);
	n->get_ntfs_usage();
	return n;
}


