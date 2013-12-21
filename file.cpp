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
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <sstream>
#include <string>

#include "file.h"
#include "log.h"
#include "main.h"
#include "log_trace.h"
#include "visitor.h"

/**
 * create
 */
FilePtr
File::create (void)
{
	FilePtr f (new File());
	f->declare ("file");
	f->weak = f;

	return f;
}

/**
 * accept
 */
bool
File::accept (Visitor& v)
{
	FilePtr f = std::dynamic_pointer_cast<File> (get_smart());
	if (!v.visit (f))
		return false;
	return visit_children (v);
}


#if 0
/**
 * find_devices
 */
bool
File::find_devices (const std::string& name, int fd, struct stat& st, ContainerPtr& list)
{
	FilePtr f (new File());
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
 * discover
 */
void
File::discover (ContainerPtr& top_level, std::queue<ContainerPtr>& probe_queue)
{
	//LOG_TRACE;

	// Config
	//	[files]
	//	dir = ~/vm
	//	file = /tmp/test.img
	//	suffix = img

	// iterate through config
	//	file => add that file
	//	dir  => add dir/*.img
}

/**
 * identify
 */
void
File::identify (ContainerPtr& top_level, const char* name, int fd, struct stat& st)
{
	//LOG_TRACE;

	/* dir:
	 *	Search for files: *.img
	 * file:
	 *	We create, and manage, a loop device for this file
	 */
}

