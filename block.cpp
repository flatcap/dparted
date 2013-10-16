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

#include <fcntl.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/major.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <sstream>
#include <string>

#include "block.h"
#include "disk.h"
#include "file.h"
#include "log.h"
#include "loop.h"
#include "main.h"
#include "log_trace.h"

/**
 * Block
 */
Block::Block (void)
{
	declare ("block", "#aaffaa");
}

/**
 * ~Block
 */
Block::~Block()
{
}


#if 0
/**
 * find_devices_old
 */
bool Block::find_devices_old (const std::string &name, DPContainer &list)
{
	struct stat st;
	int res = -1;
	//Block *item = NULL;
	long long file_size_in_bytes = 0;
	int fd = -1;

	fd = open (name.c_str(), O_RDONLY | O_CLOEXEC);
	if (fd < 0) {
		log_debug ("can't open file %s\n", name.c_str());
		return false;
	}

	res = fstat (fd, &st);
	if (res < 0) {
		log_debug ("stat on %s failed\n", name.c_str());
		close (fd);
		return false;
	}

	if (S_ISREG (st.st_mode)) {
		File::find_devices_old (name, fd, st, list);
	} else if (S_ISBLK (st.st_mode)) {
		//log_debug ("%s block\n", name.c_str());
		//log_debug ("\tdevice id = 0x%lx\n", st.st_rdev);
		if (MAJOR (st.st_rdev) == LOOP_MAJOR) {
			//log_debug ("\tloop\n");
			//item = new Loop;
			Loop::find_devices_old (name, fd, st, list);
		} else if (MAJOR (st.st_rdev) == SCSI_DISK0_MAJOR) {
			//log_debug ("\tdisk\n");
			//item = new Disk;
			Disk::find_devices_old (name, fd, st, list);
		} else {
			// exists, but I can't deal with it
			log_debug ("Unknown block device 0x%lx\n", st.st_rdev);
			return false;
		}
	} else {
		return false;
	}

	//item->find_devices_old (name, st);

	//log_debug ("fd = %d\n", fd);
	res = ioctl (fd, BLKGETSIZE64, &file_size_in_bytes); //XXX replace with ftell (user, not root)
	//log_debug ("res = %d\n", res);
	close (fd);

	//item->bytes_size = st.st_size;
	//item->bytes_size = file_size_in_bytes;
	//item->device     = name;		// This and offset should be delegated to the child
	//item->block_size = st.st_blksize;

	//log_debug ("device = %s\n", name.c_str());
	//log_debug ("size = %ld\n", st.st_size);
	//log_debug ("size = %lld\n", file_size_in_bytes);
	//log_debug ("block = %ld\n", st.st_blksize);
//	if (item) {
//		list.add_child (item);
//		queue_add_probe (item);	// queue the container for action
//	}

	// exists and I've dealt with it
	return false;
}

#endif

