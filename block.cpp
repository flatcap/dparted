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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/major.h>
#include <linux/kdev_t.h>

#include <linux/fs.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <string>

#include "block.h"
#include "disk.h"
#include "file.h"
#include "loop.h"
#include "main.h"

/**
 * Block
 */
Block::Block (void)
{
	type = "block";
}

/**
 * ~Block
 */
Block::~Block()
{
}


#if 0
/**
 * probe
 */
bool Block::probe (const std::string &name, Container &list)
{
	struct stat st;
	int res = -1;
	//Block *item = NULL;
	long long file_size_in_bytes = 0;
	int fd = -1;

	fd = open (name.c_str(), O_RDONLY);
	if (fd < 0) {
		printf ("can't open file %s\n", name.c_str());
		return false;
	}

	res = fstat (fd, &st);
	if (res < 0) {
		printf ("stat on %s failed\n", name.c_str());
		close (fd);
		return false;
	}

	if (S_ISREG (st.st_mode)) {
		File::probe (name, fd, st, list);
	} else if (S_ISBLK (st.st_mode)) {
		//printf ("%s block\n", name.c_str());
		//printf ("\tdevice id = 0x%lx\n", st.st_rdev);
		if (MAJOR (st.st_rdev) == LOOP_MAJOR) {
			//printf ("\tloop\n");
			//item = new Loop;
			Loop::probe (name, fd, st, list);
		} else if (MAJOR (st.st_rdev) == SCSI_DISK0_MAJOR) {
			//printf ("\tdisk\n");
			//item = new Disk;
			Disk::probe (name, fd, st, list);
		} else {
			// exists, but I can't deal with it
			printf ("Unknown block device 0x%lx\n", st.st_rdev);
			return false;
		}
	} else {
		return false;
	}

	//item->probe (name, st);

	//printf ("fd = %d\n", fd);
	res = ioctl (fd, BLKGETSIZE64, &file_size_in_bytes); //XXX replace with ftell (user, not root)
	//printf ("res = %d\n", res);
	close (fd);

	//item->bytes_size = st.st_size;
	//item->bytes_size = file_size_in_bytes;
	//item->device     = name;		// This and offset should be delegated to the child
	//item->block_size = st.st_blksize;

	//printf ("device = %s\n", name.c_str());
	//printf ("size = %ld\n", st.st_size);
	//printf ("size = %lld\n", file_size_in_bytes);
	//printf ("block = %ld\n", st.st_blksize);
//	if (item) {
//		list.add_child (item);
//		queue_add_probe (item);	// queue the container for action
//	}

	// exists and I've dealt with it
	return false;
}
#endif
