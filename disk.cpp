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

#include <fcntl.h>
#include <linux/fs.h>
#include <linux/major.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <sstream>
#include <string>

#include "action.h"
#include "app.h"
#include "log.h"
#include "property.h"
#include "utils.h"
#include "visitor.h"
#include "disk.h"

Disk::Disk (void)
{
	LOG_CTOR;
	const char* me = "Disk";

	sub_type (me);

	declare_prop_var (me, "bios_cylinders", bios_cylinders, "BIOS Cylinders", 0);
	declare_prop_var (me, "bios_heads",     bios_heads,     "BIOS Heads",     0);
	declare_prop_var (me, "bios_sectors",   bios_sectors,   "BIOS Sectors",   0);
	declare_prop_var (me, "did",            did,            "DID",            0);
	declare_prop_var (me, "host",           host,           "Host",           0);
	declare_prop_var (me, "hw_cylinders",   hw_cylinders,   "HW Cylinders",   0);
	declare_prop_var (me, "hw_heads",       hw_heads,       "HW Heads",       0);
	declare_prop_var (me, "hw_sectors",     hw_sectors,     "HW Sectors",     0);
	declare_prop_var (me, "mounts",         mounts,         "Mounts",         0);
	declare_prop_var (me, "read_only",      read_only,      "Read Only",      0);
}

Disk::Disk (const Disk& c) :
	Block(c)
{
	Disk();
	LOG_CTOR;
	bios_cylinders = c.bios_cylinders;
	bios_heads     = c.bios_heads;
	bios_sectors   = c.bios_sectors;
	did            = c.did;
	host           = c.host;
	hw_cylinders   = c.hw_cylinders;
	hw_heads       = c.hw_heads;
	hw_sectors     = c.hw_sectors;
	read_only      = c.read_only;
	mounts         = c.mounts;
}

Disk::Disk (Disk&& c)
{
	LOG_CTOR;
	swap (c);
}

Disk::~Disk()
{
	LOG_DTOR;
}

DiskPtr
Disk::create (void)
{
	DiskPtr p (new Disk());
	p->self = p;

	return p;
}

DiskPtr
Disk::create (const std::string& lsblk)
{
	DiskPtr d = create();

	std::map<std::string, StringNum> tags;
	int scan;

	parse_tagged_line (lsblk, " ", tags);

	if (tags["TYPE"] != "disk")
		throw "not a disk";

	d->name = tags["NAME"];
	d->device = "/dev/" + d->name;
	log_debug (d->device);

	std::string majmin = tags["MAJ:MIN"];
	scan = sscanf (majmin.c_str(), "%lu:%lu", &d->device_major, &d->device_minor);
	if (scan != 2) {
		log_debug ("scan failed1");
	}

	d->bytes_size = tags["SIZE"];
	d->mounts = tags["MOUNTPOINT"];

	return d;
}


/**
 * operator= (copy)
 */
Disk&
Disk::operator= (const Disk& c)
{
	bios_cylinders = c.bios_cylinders;
	bios_heads     = c.bios_heads;
	bios_sectors   = c.bios_sectors;
	did            = c.did;
	host           = c.host;
	hw_cylinders   = c.hw_cylinders;
	hw_heads       = c.hw_heads;
	hw_sectors     = c.hw_sectors;
	read_only      = c.read_only;
	mounts         = c.mounts;

	return *this;
}

/**
 * operator= (move)
 */
Disk&
Disk::operator= (Disk&& c)
{
	swap (c);
	return *this;
}


/**
 * swap (member)
 */
void
Disk::swap (Disk& c)
{
	std::swap (bios_cylinders, c.bios_cylinders);
	std::swap (bios_heads,     c.bios_heads);
	std::swap (bios_sectors,   c.bios_sectors);
	std::swap (did,            c.did);
	std::swap (host,           c.host);
	std::swap (hw_cylinders,   c.hw_cylinders);
	std::swap (hw_heads,       c.hw_heads);
	std::swap (hw_sectors,     c.hw_sectors);
	std::swap (read_only,      c.read_only);
	std::swap (mounts,         c.mounts);
}

/**
 * swap (global)
 */
void
swap (Disk& lhs, Disk& rhs)
{
	lhs.swap (rhs);
}


Disk*
Disk::clone (void)
{
	LOG_TRACE;
	return new Disk (*this);
}


bool
Disk::accept (Visitor& v)
{
	DiskPtr d = std::dynamic_pointer_cast<Disk> (get_smart());
	if (!v.visit(d))
		return false;

	return visit_children(v);
}


std::vector<Action>
Disk::get_actions (void)
{
	LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.disk", true },
	};

	std::vector<Action> parent_actions = Block::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
Disk::perform_action (Action action)
{
	if (action.name == "dummy.disk") {
		log_debug ("Disk perform: %s", action.name.c_str());
		return true;
	} else {
		return Block::perform_action (action);
	}
}


#if 0
bool
Disk::find_devices_old (const std::string& name, int fd, struct stat& st, ContainerPtr& list)
{
	// for /dev/sda look at
	//	/sys/block/sda/size
	//	/sys/block/sda/device/vendor
	//	/sys/block/sda/device/model

	int res;
	std::uint64_t file_size_in_bytes;

#if 0
	std::string readonly = read_file_line ("/sys/block/sda/ro");
	std::string size     = read_file_line ("/sys/block/sda/size");
	std::string model    = read_file_line ("/sys/block/sda/device/vendor");
	std::string vendor   = read_file_line ("/sys/block/sda/device/model");
#endif

	log_debug (model);
	DiskPtr d = Disk::create();

	log_debug ("fd = %d", fd);
	res = ioctl (fd, BLKGETSIZE64, &file_size_in_bytes); // replace with ftell (user, not root)
	log_debug ("res = %d", res);
	if (!res) {
	}

	d->device        = name;
	d->parent_offset = 0;
	d->bytes_size    = file_size_in_bytes;
	d->bytes_used    = 0;

	log_debug ("disk");
	log_debug ("\tname = %s", name.c_str());
	log_debug ("\tsize = %ld", file_size_in_bytes);

	list.add_child (d, true);

	return true;
}

#endif

#if 0
	int fd;
	fd = open (parent->device.c_str(), O_RDONLY | O_CLOEXEC);
	struct hd_geometry geometry;

	ioctl (fd, HDIO_GETGEO, &geometry);
	log_debug ("heads     = %d", geometry.heads);
	log_debug ("sectors   = %d", geometry.sectors);
	log_debug ("cylinders = %d", geometry.cylinders);	// truncated at ~500GiB
	// close (fd);	// or keep it for later?
#endif

std::uint64_t
Disk::get_block_size (void)
{
	return 0;
}

std::uint64_t
Disk::get_device_space (std::map<std::uint64_t, std::uint64_t>& UNUSED(spaces))
{
	return 0;
}


bool
Disk::lsblk (std::vector <std::string>& output, std::string device)
{
	// NAME="sda" MAJ:MIN="8:0" RM="0" SIZE="500107862016" RO="0" TYPE="disk" MOUNTPOINT=""
	std::string command = "sudo lsblk --bytes --pairs ";

	if (device.empty()) {
		command += "--exclude " + std::to_string (LOOP_MAJOR);
	} else {
		command += device;
	}

	command += " | grep 'TYPE=\"disk\"'";

	output.clear();
	//XXX distinguish between zero devices and an error
	int retval = execute_command_out (command, output);
	/* retval:
	 *	0 matches
	 *	0 no matches
	 *	1 invalid device
	 *	1 invalid arguments
	 */
	if (output.empty())
		return false;

	return (retval == 0);
}

void
Disk::discover (ContainerPtr& parent)
{
	return_if_fail (parent);
	LOG_TRACE;

	std::vector<std::string> output;

	if (!lsblk (output))
		return;

	for (auto& line : output) {
		DiskPtr d = Disk::create (line);

		std::string desc = "Discovered disk: " + d->get_device_short();
		parent->add_child (d, true, desc.c_str());
	}
}

bool
Disk::identify (ContainerPtr& parent, const std::string& name, int fd, struct stat& UNUSED(st))
{
	return_val_if_fail (parent, false);
	return_val_if_fail (!name.empty(), false);
	return_val_if_fail (fd>=0, false);
	LOG_TRACE;

	std::vector<std::string> output;

	if (!lsblk (output, name))	//XXX arg order? v = lsblk(n)
		return false;

	DiskPtr d = Disk::create (output[0]);

	std::string desc = "Identified disk: " + d->get_device_short();
	parent->add_child (d, true, desc.c_str());
	return true;
}


