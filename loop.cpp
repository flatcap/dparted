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

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sstream>

#include "loop.h"
#include "log_trace.h"
#include "utils.h"
#include "main.h"
#include "stringnum.h"

/**
 * Loop (void)
 */
Loop::Loop (void) :
	file_inode(0),
	file_major(0),
	file_minor(0),
	loop_major(0),
	loop_minor(0),
	offset(0),
	sizelimit(0),
	autoclear(false),
	partscan(false),
	read_only(false),
	deleted(false)
{
	declare ("loop");
}

/**
 * Loop (std::string)
 */
Loop::Loop (const std::string losetup) :
	Loop()
{
	std::vector<std::string> parts;

	explode_n (" :", losetup, parts, 12);

#if 0
	printf ("parts:\n");
	for (auto i : parts) {
		std::cout << "\t" << i << std::endl;
	}
	printf ("\n");
#endif

	//XXX validate all input, else throw()

	device     = parts[0];
	file_name  = parts[11];

	autoclear  = StringNum (parts[ 1]);
	file_inode = StringNum (parts[ 2]);
	file_major = StringNum (parts[ 3]);
	file_minor = StringNum (parts[ 4]);
	loop_major = StringNum (parts[ 5]);
	loop_minor = StringNum (parts[ 6]);
	offset     = StringNum (parts[ 7]);
	partscan   = StringNum (parts[ 8]);
	read_only  = StringNum (parts[ 9]);
	sizelimit  = StringNum (parts[10]);

	std::size_t len = file_name.size();
	if ((len > 10) && (file_name.substr (len-10) == " (deleted)")) {
		file_name.erase(len-10);
		deleted = true;
		//printf ("%s is deleted\n", device.c_str());
	}

	std::stringstream ss;
	ss << "[" << kernel_major << ":" << kernel_minor << "]";
	uuid = ss.str();

	//XXX tmp
	kernel_major = loop_major;
	kernel_minor = loop_minor;
}

/**
 * ~Loop
 */
Loop::~Loop()
{
}


/**
 * losetup
 */
bool Loop::losetup (std::vector <std::string> &output, std::string device)
{
	/* Limitations of using "losetup" output:
	 *	Filename begins, or ends, with whitespace
	 *	Filename ends " (deleted)"
	 */
	/* Alternative source of info:
	 *	/sys/devices/virtual/block/loop48/
	 *		dev
	 *		ro
	 *		size
	 *		loop/autoclear
	 *		loop/backing_file
	 *		loop/offset
	 *		loop/partscan
	 *		loop/sizelimit
	 * Need to stat backing file for inode, kernel major, kernel minor
	 */

	/* Example output:
	 *	/dev/loop17 0 47 8:3 7:272 0 1 0 0 /mnt/space/test/test_24.img
	 *	/dev/loop18 0 48 8:3 7:288 0 1 0 0 /mnt/space/test/test_30.img (deleted)
	 */


	std::string command = "losetup --noheadings --output name,autoclear,back-ino,back-maj:min,maj:min,offset,partscan,ro,sizelimit,back-file";

	if (!device.empty()) {
		command += " " + device;
	}

	output.clear();
	if (execute_command (command, output) < 0) {
		//XXX distinguish between zero loop devices and an error
		return false;
	}

	return true;
}

/**
 * discover
 */
void
Loop::discover (DPContainer &top_level, std::queue<DPContainer*> &probe_queue)
{
	std::vector <std::string> output;

	if (!losetup (output))
		return;

	for (auto line : output) {
		Loop *l = new Loop (line);

		l->open_device();

		// move to container::find_size or utils
		off_t size;
		fseek (l->fd, 0, SEEK_END);
		size = ftell (l->fd);
		l->bytes_size = size;

		top_level.add_child (l);
		probe_queue.push (l);	// We need to probe
	}
}

/**
 * identify
 */
void
Loop::identify (DPContainer &top_level, const char *name, int fd, struct stat &st)
{
	//LOG_TRACE;

	Loop *l = NULL;
	off_t size;

	std::vector <std::string> output;

	losetup (output, name);		//XXX retval, exactly one reply

	//std::cout << output[0] << std::endl;

	l = new Loop (output[0]);

	size = lseek (fd, 0, SEEK_END);

	l->device        = name;
	l->parent_offset = 0;
	l->bytes_size    = size;
	l->bytes_used    = 0;

	size_t i = l->device.find_last_of ('/');
	if (i == std::string::npos) {
		l->name = l->device;
	} else {
		l->name = l->device.substr(i+1);
	}

	l->block_size = 512;	//XXX granularity, or blocksize of backing-file fs/disk?

	std::stringstream ss;
	ss << "[" << l->loop_major << ":" << l->loop_minor << "]";
	l->uuid = ss.str();

	top_level.add_child (l);
	queue_add_probe (l);	// queue the container for action
}

