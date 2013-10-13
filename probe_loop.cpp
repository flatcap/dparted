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

#include <string>
#include <vector>
#include <cstring>

#include "probe_loop.h"
#include "loop.h"

#include "utils.h"
#include "log.h"

static ProbeLoop *prober = NULL;

/**
 * ProbeLoop
 */
ProbeLoop::ProbeLoop()
{
	name = "loop",
	description = "Loopback devices";
}

/**
 * ~ProbeLoop
 */
ProbeLoop::~ProbeLoop()
{
}


/**
 * shutdown
 */
void ProbeLoop::shutdown (void)
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
ProbeLoop::discover (void)
{
	children.clear();	// XXX scan and update existing Loop objects

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

#if 0
		l->open_device();

		// move to container::find_size or utils
		long long seek;
		fseek (l->fd, 0, SEEK_END);
		seek = ftell (l->fd);
		l->bytes_size = seek;
#endif

		children.insert (l);
	}

	for (auto c : children) {
		printf ("%s\n", c->device.c_str());
	}
}

