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

#include <unistd.h>

#include "loop.h"
#include "log_trace.h"
#include "utils.h"
#include "main.h"

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


/**
 * discover
 */
void
Loop::discover (DPContainer &top_level, std::queue<DPContainer*> &probe_queue)
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
	std::vector <std::string> output;

	if (execute_command (command, output) < 0) {
		//XXX distinguish between zero loop devices and an error
		return;
	}

	std::vector<std::string> parts;
	int tmp;

	for (auto line : output) {
		explode_n (" :", line, parts, 12);

		//XXX validate all input, before creating Loop
		Loop *l = new Loop();

		l->device    = parts[0];
		l->file_name = parts[11];

		// XXX utils StringNum
		tmp = -1; sscanf (parts[ 1].c_str(), "%d", &tmp); l->autoclear  = (tmp == 1);
		tmp = -1; sscanf (parts[ 2].c_str(), "%d", &tmp); l->file_inode = tmp;
		tmp = -1; sscanf (parts[ 3].c_str(), "%d", &tmp); l->file_major = tmp;
		tmp = -1; sscanf (parts[ 4].c_str(), "%d", &tmp); l->file_minor = tmp;
		tmp = -1; sscanf (parts[ 5].c_str(), "%d", &tmp); l->loop_major = tmp;
		tmp = -1; sscanf (parts[ 6].c_str(), "%d", &tmp); l->loop_minor = tmp;
		tmp = -1; sscanf (parts[ 7].c_str(), "%d", &tmp); l->offset     = tmp;
		tmp = -1; sscanf (parts[ 8].c_str(), "%d", &tmp); l->partscan   = (tmp == 1);
		tmp = -1; sscanf (parts[ 9].c_str(), "%d", &tmp); l->read_only  = (tmp == 1);
		tmp = -1; sscanf (parts[10].c_str(), "%d", &tmp); l->sizelimit  = tmp;

		std::size_t len = l->file_name.size();
		if ((len > 10) && (l->file_name.substr (len-10) == " (deleted)")) {
			l->file_name.erase(len-10);
			l->deleted = true;
			printf ("%s is deleted\n", l->device.c_str());
		}

		//XXX tmp
		l->kernel_major = l->loop_major;
		l->kernel_minor = l->loop_minor;

#if 1
		l->open_device();

		// move to container::find_size or utils
		long long seek;
		fseek (l->fd, 0, SEEK_END);
		seek = ftell (l->fd);
		l->bytes_size = seek;
#endif

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
	long long seek;

	seek = lseek (fd, 0, SEEK_END);

	l = new Loop;

	l->device        = name;
	l->parent_offset = 0;
	l->bytes_size    = seek;
	l->bytes_used    = 0;

	top_level.add_child (l);
	queue_add_probe (l);	// queue the container for action
}

