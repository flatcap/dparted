/* Copyright (c) 2012 Richard Russon (FlatCap)
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


#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cstdio>
#include <sstream>
#include <string>

#include "log.h"
#include "loop.h"
#include "main.h"
#include "utils.h"

/**
 * Loop
 */
Loop::Loop (void)
{
	declare ("loop");
}

/**
 * ~Loop
 */
Loop::~Loop()
{
}


#if 0
/**
 * probe
 */
bool Loop::probe (const std::string &name, int fd, struct stat &st, DPContainer &list)
{
	Loop *l = NULL;
	long long seek;

	seek = lseek (fd, 0, SEEK_END);

	l = new Loop;

	l->device        = name;
	l->parent_offset = 0;
	l->bytes_size    = seek;
	l->bytes_used    = 0;

	list.add_child (l);
	queue_add_probe (l);	// queue the container for action

	return true;
}

#endif

/**
 * find_devices
 */
unsigned int Loop::find_devices (DPContainer &list)
{

	// /dev/loop0: [0831]:4457032 (/home/flatcap/work/partitions/images/disk0)
	std::string command = "losetup -a";
	std::vector <std::string> output;
	std::string error;
	unsigned int count;

	count = execute_command (command, output);
	if (count < 0)
		return 0;

	//log_debug ("%s\n", output.c_str());

	std::string device;
	std::string file;
	int kernel_major = -1;
	int kernel_minor = -1;
	long inode;
	size_t pos;
	unsigned int i;
	std::string part;
	int scan;
	int added = 0;

	//log_debug ("%d lines\n", count);

	for (i = 0; i < count; i++) {
		//log_debug ("line = >>%s<<\n", output[i].c_str());
		pos = output[i].find (": [");
		if (pos == std::string::npos) {
			log_debug ("corrupt line1\n");
			continue;
		}
		device = output[i].substr (0, pos);
		//log_debug ("%s\n", device.c_str());

		part = output[i].substr (pos + 3, 4);
		//log_debug ("%s\n", part.c_str());
		scan = sscanf (part.c_str(), "%02x%02x", &kernel_major, &kernel_minor);
		if (scan != 2) {
			log_debug ("scan failed1\n");
			continue;
		}
		//log_debug ("\tmajor: %d\n", kernel_major);
		//log_debug ("\tminor: %d\n", kernel_minor);

		part = output[i].substr (pos + 9);

		pos = part.find (" (");
		if (pos == std::string::npos) {
			log_debug ("corrupt line2\n");
			continue;
		}

		part = part.substr (0, pos);
		//log_debug ("part = %s\n", part.c_str());

		scan = sscanf (part.c_str(), "%ld", &inode);
		if (scan != 1) {
			log_debug ("scan failed2\n");
			continue;
		}
		//log_debug ("\tinode: %ld\n", inode);

		pos = output[i].find (" (");
		if (pos == std::string::npos) {
			log_debug ("corrupt line3\n");
			continue;
		}

		part = output[i].substr (pos + 2);
		//log_debug ("part = %s\n", part.c_str());

		pos = part.length();
		if (part[pos - 1] != ')') {
			log_debug ("corrupt line4\n");
			continue;
		}

		part = part.substr (0, pos - 1);
		//log_debug ("\tfile:  %s\n", part.c_str());

		//log_debug ("\n");

		Loop *l = new Loop;
		l->device = device;
		l->parent_offset = 0;
		l->kernel_major = kernel_major;
		l->kernel_minor = kernel_minor;
		l->inode = inode;
		l->file = file;

		l->open_device();

		// move to container::find_size or utils
		long long seek;
		fseek (l->fd, 0, SEEK_END);
		seek = ftell (l->fd);
		l->bytes_size = seek;

		list.add_child (l);
		added++;
	}

	return added;
}


/**
 * dump_dot
 */
std::string Loop::dump_dot (void)
{
	std::ostringstream output;

	// no specifics for now

	output << Block::dump_dot();

	return output.str();
}

