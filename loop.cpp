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
	log_info ("parts:\n");
	for (auto i : parts) {
		std::cout << "\t" << i << std::endl;
	}
	log_info ("\n");
#endif

	//XXX validate all input, else throw()

	device     = parts[0];
	file_name  = parts[11];

	name = device;
	size_t index = name.find_last_of ('/');
	if (index != std::string::npos) {
		name = name.substr (index+1);
	}

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
		//log_info ("%s is deleted\n", device.c_str());
	}

	//XXX tmp
	kernel_major = loop_major;
	kernel_minor = loop_minor;
	block_size   = 512;	//XXX kernel limit, but fs block size is likely to be bigger

	std::stringstream ss;
	ss << "[" << kernel_major << ":" << kernel_minor << "]";
	uuid = ss.str();
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
bool
Loop::losetup (std::vector <std::string>& output, std::string device)
{
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

	/* Limitations of using "losetup" output:
	 *	Filename begins, or ends, with whitespace
	 *	Filename ends " (deleted)"
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
Loop::discover (ContainerPtr& top_level, std::queue<ContainerPtr>& probe_queue)
{
	std::vector <std::string> output;

	if (!losetup (output))
		return;

	for (auto line : output) {
		LoopPtr l (new Loop (line));

		l->get_fd();

		// move to container::find_size or utils (or block::)
		off_t size;
		size = lseek (l->fd, 0, SEEK_END);
		l->bytes_size = size;

		ContainerPtr c(l);
		top_level->just_add_child (c);
		probe_queue.push (c);	// We need to probe
	}
}

/**
 * identify
 */
void
Loop::identify (ContainerPtr& top_level, const char* name, int fd, struct stat& st)
{
	//LOG_TRACE;

	off_t size;

	std::vector <std::string> output;

	losetup (output, name);		//XXX retval, exactly one reply

	//std::cout << output[0] << std::endl;

	LoopPtr l (new Loop (output[0]));

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

	ContainerPtr c(l);
	top_level->just_add_child (c);
	queue_add_probe (c);	// queue the container for action
}

