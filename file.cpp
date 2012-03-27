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
#include <linux/fs.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <sstream>
#include <string>

#include "file.h"
#include "log.h"
#include "main.h"

/**
 * File
 */
File::File (void)
{
	declare ("file");
}

/**
 * ~File
 */
File::~File()
{
}


#if 0
/**
 * probe
 */
bool File::probe (const std::string &name, int fd, struct stat &st, DPContainer &list)
{
	File *f = NULL;

	f = new File;
#if 0
	log_debug ("dev     = 0x%04lx\n", st.st_dev);
	log_debug ("ino     = %ld\n",     st.st_ino);
	log_debug ("nlink   = %ld\n",     st.st_nlink);
	log_debug ("mode    = %06o\n",    st.st_mode);
	log_debug ("uid     = %d\n",      st.st_uid);
	log_debug ("gid     = %d\n",      st.st_gid);
	log_debug ("rdev    = %ld\n",     st.st_rdev);
	log_debug ("size    = %ld\n",     st.st_size);
	log_debug ("blksize = %ld\n",     st.st_blksize);
	log_debug ("blocks  = %ld\n",     st.st_blocks);
	log_debug ("atime   = %ld\n",     st.st_atim.tv_sec);
	log_debug ("mtime   = %ld\n",     st.st_mtim.tv_sec);
	log_debug ("ctime   = %ld\n",     st.st_ctim.tv_sec);
#endif

	f->device        = name;
	f->parent_offset = 0;
	f->bytes_size    = st.st_size;
	f->bytes_used    = 0;

	list.add_child (f);
	queue_add_probe (f);	// queue the container for action

	return true;
}

#endif

/**
 * dump_dot
 */
std::string File::dump_dot (void)
{
	std::ostringstream output;

	// no specifics for now

	output << Block::dump_dot();

	return output.str();
}

