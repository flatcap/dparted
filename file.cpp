/* Copyright (c) 2014 Richard Russon (FlatCap)
 *
 * This file is part of DParted.
 *
 * DParted is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DParted is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DParted.  If not, see <http://www.gnu.org/licenses/>.
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
 * File
 */
File::File (void)
{
	const char* me = "File";

	sub_type (me);
}

/**
 * create
 */
FilePtr
File::create (void)
{
	FilePtr f (new File());
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
	if (!v.visit(f))
		return false;
	return visit_children(v);
}


#if 0
/**
 * find_devices
 */
bool
File::find_devices (const std::string& name, int fd, struct stat& st, ContainerPtr& list)
{
	FilePtr f = File::create();
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

	list.add_child(f);
	queue_add_probe(f);	// queue the container for action

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

/**
 * get_actions
 */
std::vector<Action>
File::get_actions (void)
{
	// LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.file", true },
	};

	std::vector<Action> parent_actions = Block::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

/**
 * perform_action
 */
bool
File::perform_action (Action action)
{
	if (action.name == "dummy.file") {
		std::cout << "File perform: " << action.name << std::endl;
		return true;
	} else {
		return Block::perform_action (action);
	}
}


