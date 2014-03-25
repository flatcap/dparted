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

#include <vector>
#include <map>
#include <algorithm>
#include <cstring>

#include "ntfs.h"
#include "action.h"
#include "log_trace.h"
#include "utils.h"
#include "visitor.h"

Ntfs::Ntfs (void)
{
	const char* me = "Ntfs";

	sub_type (me);
}

Ntfs::~Ntfs()
{
}

NtfsPtr
Ntfs::create (void)
{
	NtfsPtr p (new Ntfs());
	p->weak = p;

	return p;
}


bool
Ntfs::accept (Visitor& v)
{
	NtfsPtr b = std::dynamic_pointer_cast<Ntfs> (get_smart());
	if (!v.visit(b))
		return false;
	return visit_children(v);
}


std::vector<Action>
Ntfs::get_actions (void)
{
	// LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.ntfs", true },
	};

	std::vector<Action> parent_actions = Filesystem::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
Ntfs::perform_action (Action action)
{
	if (action.name == "dummy.ntfs") {
		std::cout << "Ntfs perform: " << action.name << std::endl;
		return true;
	} else {
		return Filesystem::perform_action (action);
	}
}


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


void
Ntfs::get_ntfs_sb (ContainerPtr parent)
{
	//XXX return bool -- a quick match on the sb might not be enough -- ntfsinfo could fail
	if (!parent)
		return;

	std::string dev = parent->get_device_name();
	if (dev.empty())	//XXX shouldn't happen
		return;

	std::string command;
	std::vector<std::string> output;

	command = "ntfsinfo --mft " + dev;
	execute_command1 (command, output);	//XXX return value?

	more_props.reserve (output.size());	// if this vector is reallocated the app will die

	std::string key;
	std::string value;
	std::string desc;
	std::string section;

	long clust = -1;	// cluster size
	long csize = -1;	// total size in clusters
	long cfree = -1;	// free space in clusters

	//log_debug ("keys:\n");
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
		declare_prop_array (section.c_str(), key.c_str(), more_props, more_props.size()-1, desc.c_str());
	}

	//log_debug ("%ld, %ld, %ld\n", clust, csize, cfree);
	if ((clust > 0) && (csize > 0) && (cfree > 0)) {
		block_size = clust;
		bytes_size = csize * clust;
		bytes_used = bytes_size - (cfree * clust);
	} else {
		//XXX failed to get vital info
	}
}

NtfsPtr
Ntfs::get_ntfs (ContainerPtr parent, std::uint8_t* buffer, std::uint64_t bufsize)
{
	//LOG_TRACE;

	if (!parent || !buffer || !bufsize)
		return nullptr;

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


