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
#include <linux/fs.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <string>
#include <sstream>

#include "file.h"
#include "main.h"

/**
 * File
 */
File::File (void)
{
	name = "file";
	type = "file";
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
bool File::probe (const std::string &name, int fd, struct stat &st, Container &list)
{
	File *f = NULL;

	f = new File;
#if 0
	printf ("dev     = 0x%04lx\n", st.st_dev);
	printf ("ino     = %ld\n",     st.st_ino);
	printf ("nlink   = %ld\n",     st.st_nlink);
	printf ("mode    = %06o\n",    st.st_mode);
	printf ("uid     = %d\n",      st.st_uid);
	printf ("gid     = %d\n",      st.st_gid);
	printf ("rdev    = %ld\n",     st.st_rdev);
	printf ("size    = %ld\n",     st.st_size);
	printf ("blksize = %ld\n",     st.st_blksize);
	printf ("blocks  = %ld\n",     st.st_blocks);
	printf ("atime   = %ld\n",     st.st_atim.tv_sec);
	printf ("mtime   = %ld\n",     st.st_mtim.tv_sec);
	printf ("ctime   = %ld\n",     st.st_ctim.tv_sec);
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

	output << dump_table_header ("File", "#00ff88");

	// no specfics for now

	output << Block::dump_dot();

	output << dump_table_footer();
	output << dump_dot_children();

	return output.str();
}

