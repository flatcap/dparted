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

#include "probe_disk.h"
#include "disk.h"

#include "utils.h"
#include "log.h"
#include "log_trace.h"

static ProbeDisk *prober = NULL;

/**
 * ProbeDisk
 */
ProbeDisk::ProbeDisk()
{
	name = "loop",
	description = "Diskback devices";
}

/**
 * ~ProbeDisk
 */
ProbeDisk::~ProbeDisk()
{
}


/**
 * shutdown
 */
void ProbeDisk::shutdown (void)
{
	if (prober) {
		delete prober;
		prober = NULL;
	}
}

/**
 * discover
 */
void
ProbeDisk::discover (void)
{
	//LOG_TRACE;

	children.clear();	// XXX scan and update existing Disk objects

	// NAME="sda" MAJ:MIN="8:0" RM="0" SIZE="500107862016" RO="0" TYPE="disk" MOUNTPOINT=""
	std::string command = "lsblk -b -P -e 7";
	std::vector<std::string> output;
	std::string error;
	unsigned int count;

	count = execute_command (command, output);
	if (count < 0)
		return;

#if 0
	for (auto line : output) {
		log_debug ("%s\n", line.c_str());
	}
#endif

	std::string device;
	std::string type;
	std::string mount;
	int kernel_major = -1;
	int kernel_minor = -1;
	long long size;
	std::string part;
	int scan;
	std::map<std::string,StringNum> tags;

	//log_debug ("%d lines\n", count);

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

		Disk *d = new Disk;
		d->device = "/dev/" + device;
		d->parent_offset = 0;
		d->kernel_major = kernel_major;
		d->kernel_minor = kernel_minor;
		d->mounts = mount;
		d->bytes_size = size;

		d->open_device();

		children.insert (d);
	}

	for (auto c : children) {
		printf ("%s\n", c->device.c_str());
	}
}

