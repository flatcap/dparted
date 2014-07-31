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

#include "action.h"
#include "btrfs.h"
#include "endian.h"
#include "utils.h"
#include "visitor.h"
#include "log.h"

Btrfs::Btrfs (void)
{
	LOG_CTOR;
	const char* me = "Btrfs";

	sub_type (me);
}

Btrfs::Btrfs (const Btrfs& c) :
	Filesystem(c)
{
	Btrfs();
	LOG_CTOR;
	// No properties
}

Btrfs::Btrfs (Btrfs&& c)
{
	LOG_CTOR;
	swap(c);
}

Btrfs::~Btrfs()
{
	LOG_DTOR;
}

BtrfsPtr
Btrfs::create (void)
{
	BtrfsPtr p (new Btrfs());
	p->self = p;

	return p;
}


Btrfs&
Btrfs::operator= (const Btrfs& UNUSED(c))
{
	// No properties

	return *this;
}

Btrfs&
Btrfs::operator= (Btrfs&& c)
{
	swap(c);
	return *this;
}


void
Btrfs::swap (Btrfs& UNUSED(c))
{
	// No properties
}

void
swap (Btrfs& lhs, Btrfs& rhs)
{
	lhs.swap (rhs);
}


Btrfs*
Btrfs::clone (void)
{
	LOG_TRACE;
	return new Btrfs (*this);
}


bool
Btrfs::accept (Visitor& v)
{
	BtrfsPtr b = std::dynamic_pointer_cast<Btrfs> (get_smart());
	if (!v.visit(b))
		return false;

	return visit_children(v);
}


std::vector<Action>
Btrfs::get_actions (void)
{
	LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.btrfs", true },
	};

	std::vector<Action> parent_actions = Filesystem::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
Btrfs::perform_action (Action action)
{
	if (action.name == "dummy.btrfs") {
		log_debug ("Btrfs perform: %s", action.name.c_str());
		return true;
	} else {
		return Filesystem::perform_action (action);
	}
}


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

bool
parse_header (const std::string& header, std::string& dev)
{
	// "superblock: bytenr=65536, device=/dev/loop1p1"

	if (header.substr (0, 19) != "superblock: bytenr=")
		return false;

	std::size_t pos = header.find ("device=");
	if (pos == std::string::npos)
		return false;

	dev = header.substr (pos+7);
	return true;
}

static bool
parse_line (const std::string& line, std::string& key, std::string& value)
{
	// Up to 21 chars, tab(s), value
	std::size_t pos;
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

static std::string
make_desc (std::string key)
{
	// underscore/dot -> space
	std::size_t pos = -1;
	do {
		pos = key.find_first_of ("_.", pos+1);
		if (pos != std::string::npos) {
			key[pos] = ' ';
		}

	} while (pos != std::string::npos);

	key[0] = ::toupper (key[0]);
	return key;
}

std::map<std::string, std::string>
btrfs_show_super (const std::string& dev)
{
	std::string command;
	std::vector<std::string> output;
	std::map<std::string, std::string> results;

	command = "btrfs-show-super " + dev;
	int retval = execute_command_out (command, output);
	/* retval:
	 *	0 match,    magic _BHRfS_M [match]
	 *	0 no match, magic ........ [DON'T MATCH]
	 *	1 device doesn't exist
	 *	1 invalid arguments
	 */

	if (retval != 0) {
		return {};
	}

	std::string key;
	std::string value;

	log_debug ("keys:");
	for (auto& line : output) {
		//XXX move these two tests before loop and doctor vector
		if (line.substr (0, 11) == "superblock:") {
			std::string dev2;
			if (!parse_header (line, dev2)) {
				log_debug ("btrfs: bad header");
				break;
			}
			if (dev != dev2) {
				log_debug ("btrfs: devices don't match");
				break;
			}
			continue;
		} else if (line.substr (0, 10) == "----------") {
			continue;
		} else if (line.empty()) {
			continue;
		} else if (!parse_line (line, key, value)) {
			log_debug ("btrfs failed: %s", line.c_str());
			continue;
		}

		log_debug ("\t%s", key.c_str());
		results[key] = value;
	}

	return results;
}

void
Btrfs::get_btrfs_sb (ContainerPtr parent)
{
	//XXX return bool -- a quick match on the sb might not be enough -- tune2fs could fail
	return_if_fail (parent);

	std::string dev = parent->get_device_name();
	if (dev.empty())	//XXX shouldn't happen
		return;

	std::map<std::string, std::string> info = btrfs_show_super (dev);
	if (info.empty()) {
		log_debug ("btrfs_show_super failed");
		return;
	}

	// declare everything else
	const char* me = "Btrfs";
	more_props.reserve (info.size());	// if this vector is reallocated the app will die
	log_debug ("Props:");
	for (auto& i : info) {
		std::string key   = "btrfs." + i.first;
		std::string desc  = make_desc (i.first);
		std::string value = i.second;
		log_debug ("\t%-32s %-24s %s", key.c_str(), desc.c_str(), value.c_str());

		more_props.push_back (value);
		declare_prop_array (me, key.c_str(), more_props, more_props.size()-1, desc.c_str(), 0);
	}
}

BtrfsPtr
Btrfs::get_btrfs (ContainerPtr parent, std::uint8_t* buffer, int bufsize)
{
	return_val_if_fail (parent, nullptr);
	return_val_if_fail (buffer, nullptr);
	LOG_TRACE;

	if (bufsize < 1048576)		// Min btrfs size is 1MiB
		return nullptr;

	if (strncmp ((char*) buffer+65600, "_BHRfS_M", 8) != 0)
		return nullptr;

	BtrfsPtr b = Btrfs::create();
	b->sub_type ("btrfs");

	b->name = get_fixed_str (buffer+0x1012B, 255);
	b->uuid = read_uuid1 (buffer + 0x10020);

	b->bytes_size = le64_to_cpup (buffer + 0x10070);
	b->bytes_used = le64_to_cpup (buffer + 0x10078);
	b->block_size = le32_to_cpup (buffer + 0x10090);

	b->get_btrfs_sb (parent);
	b->get_btrfs_usage();
	return b;
}


