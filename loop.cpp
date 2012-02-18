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


#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <string>
#include <sstream>

#include "loop.h"
#include "main.h"
#include "utils.h"

/**
 * Loop
 */
Loop::Loop (void)
{
	name = "loop";
	type = "loop";
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
bool Loop::probe (const std::string &name, int fd, struct stat &st, Container &list)
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
unsigned int Loop::find_devices (Container &list)
{
	int retval = -1;

	// /dev/loop0: [0831]:4457032 (/home/flatcap/work/partitions/images/disk0)
	std::string command = "losetup -a";
	std::string output;
	std::string error;

	retval = execute_command (command, output, error);
	if (retval < 0)
		return 0;

	//printf ("%s\n", output.c_str());

	std::string device;
	std::string file;
	int kernel_major = -1;
	int kernel_minor = -1;
	long inode;
	unsigned int count;
	size_t pos;
	std::vector<std::string> lines;
	unsigned int i;
	std::string part;
	int scan;
	int added = 0;

	count = explode ("\n", output, lines);
	//printf ("%d lines\n", count);

	for (i = 0; i < count; i++) {
		pos = lines[i].find (": [");
		if (pos == std::string::npos) {
			printf ("corrupt line1\n");
			continue;
		}
		device = lines[i].substr (0, pos);
		//printf ("%s\n", device.c_str());

		part = lines[i].substr (pos + 3, 4);
		//printf ("%s\n", part.c_str());
		scan = sscanf (part.c_str(), "%02x%02x", &kernel_major, &kernel_minor);
		if (scan != 2) {
			printf ("scan failed1\n");
			continue;
		}
		//printf ("\tmajor: %d\n", kernel_major);
		//printf ("\tminor: %d\n", kernel_minor);

		part = lines[i].substr (pos + 9);

		pos = part.find (" (");
		if (pos == std::string::npos) {
			printf ("corrupt line2\n");
			continue;
		}

		part = part.substr (0, pos);
		//printf ("part = %s\n", part.c_str());

		scan = sscanf (part.c_str(), "%ld", &inode);
		if (scan != 1) {
			printf ("scan failed2\n");
			continue;
		}
		//printf ("\tinode: %ld\n", inode);

		pos = lines[i].find (" (");
		if (pos == std::string::npos) {
			printf ("corrupt line3\n");
			continue;
		}

		part = lines[i].substr (pos + 2);
		//printf ("part = %s\n", part.c_str());

		pos = part.length();
		if (part[pos - 1] != ')') {
			printf ("corrupt line4\n");
			continue;
		}

		part = part.substr (0, pos - 1);
		//printf ("\tfile:  %s\n", part.c_str());

		//printf ("\n");

		Loop *l = new Loop;
		l->device = device;
		l->parent_offset = 0;
		l->kernel_major = kernel_major;
		l->kernel_minor = kernel_minor;
		l->inode = inode;
		l->file = file;

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

	output << dump_table_header ("Loop", "#00ff88");

	// no specfics for now

	output << Block::dump_dot();

	output << dump_table_footer();
	output << dump_dot_children();

	return output.str();
}

