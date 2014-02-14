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
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cstdio>
#include <sstream>
#include <string>

#include "app.h"
#include "disk.h"
#include "log.h"
#include "main.h"
#include "utils.h"
#include "log_trace.h"
#include "visitor.h"

/**
 * Disk
 */
Disk::Disk (void)
{
	const char* me = "Disk";

	sub_type (me);

	declare_prop (me, "read_only",      read_only,      "desc of read_only");
	declare_prop (me, "hw_cylinders",   hw_cylinders,   "desc of hw_cylinders");
	declare_prop (me, "hw_heads",       hw_heads,       "desc of hw_heads");
	declare_prop (me, "hw_sectors",     hw_sectors,     "desc of hw_sectors");
	declare_prop (me, "bios_cylinders", bios_cylinders, "desc of bios_cylinders");
	declare_prop (me, "bios_heads",     bios_heads,     "desc of bios_heads");
	declare_prop (me, "bios_sectors",   bios_sectors,   "desc of bios_sectors");
	declare_prop (me, "host",           host,           "desc of host");
	declare_prop (me, "did",            did,            "desc of did");
	declare_prop (me, "mounts",         mounts,         "desc of mounts");
}

/**
 * create
 */
DiskPtr
Disk::create (void)
{
	DiskPtr d (new Disk());
	d->weak = d;

	return d;
}

/**
 * create
 */
DiskPtr
Disk::create (const std::string& lsblk)
{
	DiskPtr d = create();

	std::map<std::string,StringNum> tags;
	int scan;

	parse_tagged_line (lsblk, " ", tags);

	if (tags["TYPE"] != "disk")
		throw "not a disk";

	d->name = tags["NAME"];
	d->device = "/dev/" + d->name;
	//log_debug ("%s\n", device.c_str());

	std::string majmin = tags["MAJ:MIN"];
	scan = sscanf (majmin.c_str(), "%d:%d", &d->kernel_major, &d->kernel_minor);
	if (scan != 2) {
		log_debug ("scan failed1\n");
	}

	d->bytes_size = tags["SIZE"];
	d->mounts = tags["MOUNTPOINT"];

	return d;
}


/**
 * accept
 */
bool
Disk::accept (Visitor& v)
{
	DiskPtr d = std::dynamic_pointer_cast<Disk> (get_smart());
	if (!v.visit(d))
		return false;
	return visit_children(v);
}


#if 0
/**
 * find_devices_old
 */
bool
Disk::find_devices_old (const std::string& name, int fd, struct stat& st, ContainerPtr& list)
{
	// for /dev/sda look at
	//	/sys/block/sda/size
	//	/sys/block/sda/device/vendor
	//	/sys/block/sda/device/model

	int res;
	long file_size_in_bytes;

#if 0
	std::string readonly = read_file_line ("/sys/block/sda/ro");
	std::string size     = read_file_line ("/sys/block/sda/size");
	std::string model    = read_file_line ("/sys/block/sda/device/vendor");
	std::string vendor   = read_file_line ("/sys/block/sda/device/model");
#endif

	//log_debug ("%s\n", model.c_str());
	DiskPtr d = Disk::create();

	//log_debug ("fd = %d\n", fd);
	res = ioctl (fd, BLKGETSIZE64, &file_size_in_bytes); //XXX replace with ftell (user, not root)
	//log_debug ("res = %d\n", res);
	if (!res) {
	}

	d->device        = name;
	d->parent_offset = 0;
	d->bytes_size    = file_size_in_bytes;
	d->bytes_used    = 0;

#if 0
	log_debug ("disk\n");
	log_debug ("\tname = %s\n", name.c_str());
	log_debug ("\tsize = %lld\n", file_size_in_bytes);
#endif

	list.add_child(d);
	queue_add_probe(d);	// queue the container for action

	return true;
}

#endif

#if 0
	int fd;
	fd = open (parent->device.c_str(), O_RDONLY | O_CLOEXEC);
	struct hd_geometry geometry;

	ioctl (fd, HDIO_GETGEO, &geometry);
	log_debug ("heads     = %d\n", geometry.heads);
	log_debug ("sectors   = %d\n", geometry.sectors);
	log_debug ("cylinders = %d\n", geometry.cylinders);	// truncated at ~500GiB
	//close (fd);	// XXX or keep it for later?
#endif

/**
 * find_devices
 */
unsigned int
Disk::find_devices (ContainerPtr& list)
{
	// NAME="sda" MAJ:MIN="8:0" RM="0" SIZE="500107862016" RO="0" TYPE="disk" MOUNTPOINT=""
	//XXX use LOOP_MAJOR
	std::string command = "lsblk -b -P -e 7";
	std::vector<std::string> output;
	std::string error;

	execute_command1 (command, output);
	if (output.empty())
		return 0;

	//log_debug ("%s\n", output.c_str());

	std::string device;
	std::string type;
	std::string mount;
	int kernel_major = -1;
	int kernel_minor = -1;
	long size;
	std::string part;
	int scan;
	std::map<std::string,StringNum> tags;
	int added = 0;

	//log_debug ("%d lines\n", output.size());

	for (auto line : output) {
		parse_tagged_line (line, " ", tags);

		type = tags["TYPE"];
		if (type != "disk")
			continue;

		device = tags["NAME"];
		//log_debug ("%s\n", device.c_str());

		std::string majmin = tags["MAJ:MIN"];
		scan = sscanf (majmin.c_str(), "%d:%d", &kernel_major, &kernel_minor);
		if (scan != 2) {
			log_debug ("scan failed1\n");
			continue;
		}

		size = tags["SIZE"];
		mount = tags["MOUNTPOINT"];

#if 0
		log_debug ("\tmajor: %d\n", kernel_major);
		log_debug ("\tminor: %d\n", kernel_minor);
		log_debug ("\tsize:  %lld\n", size);
		log_debug ("\tmount: %s\n", mount.c_str());
		log_debug ("\n");
#endif

		DiskPtr d = Disk::create();
		d->device = "/dev/" + device;
		d->parent_offset = 0;
		d->kernel_major = kernel_major;
		d->kernel_minor = kernel_minor;
		d->mounts = mount;
		d->bytes_size = size;

		//d->open_device();

		ContainerPtr c(d);
		list->add_child(c);
		added++;
	}

	return added;
}


/**
 * get_block_size
 */
long
Disk::get_block_size (void)
{
	return 0;
}

/**
 * get_device_space
 */
unsigned int
Disk::get_device_space (std::map<long, long>& spaces)
{
	return 0;
}


/**
 * find_device
 */
ContainerPtr
Disk::find_device (const std::string& dev)
{
	// does it sound like one of my children?  /dev/sdaX, /dev/sdaXX
	unsigned int dev_len = device.length();

	// iterate through my children
	if (device.compare (0, dev_len, dev, 0, dev_len) == 0) {
		//log_debug ("similar\n");
		return Block::find_device (dev);
	}

	return nullptr;
}


/**
 * lsblk
 */
bool
Disk::lsblk (std::vector <std::string>& output, std::string device)
{
	// NAME="sda" MAJ:MIN="8:0" RM="0" SIZE="500107862016" RO="0" TYPE="disk" MOUNTPOINT=""
	std::string command = "sudo lsblk -b -P ";

	if (device.empty()) {
		command += "-e 7";	//XXX LOOP_MAJOR
	} else {
		command += device;
	}

	command += " | grep 'TYPE=\"disk\"'";

	output.clear();
	//XXX distinguish between zero loop devices and an error
	execute_command1 (command, output);
	if (output.empty())
		return false;

	return true;
}

/**
 * discover
 */
void
Disk::discover (ContainerPtr& top_level, std::queue<ContainerPtr>& probe_queue)
{
	//LOG_TRACE;

	std::vector<std::string> output;

	if (!lsblk (output))
		return;

	//log_debug ("%d lines\n", count);

	for (auto line : output) {
		DiskPtr d = Disk::create (line);

		ContainerPtr c(d);
		top_level->just_add_child(c);
		probe_queue.push(c);	// We need to probe
	}
}

/**
 * identify
 */
void
Disk::identify (ContainerPtr& top_level, const char* name, int fd, struct stat& st)
{
	//LOG_TRACE;
	std::vector<std::string> output;

	if (!lsblk (output, name))
		return;

	DiskPtr d = Disk::create (output[0]);

	ContainerPtr c(d);
	top_level->just_add_child(c);
	main_app->queue_add_probe(c);	// queue the container for action
}


/**
 * get_actions
 */
std::vector<Action>
Disk::get_actions (void)
{
	// LOG_TRACE;
	std::vector<Action> actions = {
		//{ "create.filesystem", true },
	};

	std::vector<Action> parent_actions = Block::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

/**
 * perform_action
 */
bool
Disk::perform_action (Action action)
{
	if (action.name == "create.table") {
		std::cout << "Disk perform: " << action.name << std::endl;
		return true;
	} else {
		std::cout << "Unknown action: " << action.name << std::endl;
		return false;
	}
}


