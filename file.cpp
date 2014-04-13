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
#include "action.h"
#include "log.h"
#include "log_trace.h"
#include "main.h"
#include "visitor.h"
#include "log.h"

File::File (void)
{
	log_ctor ("ctor File");
	const char* me = "File";

	sub_type (me);
}

File::~File()
{
	log_dtor ("dtor File");
}

FilePtr
File::create (void)
{
	FilePtr p (new File());
	p->self = p;

	return p;
}


bool
File::accept (Visitor& v)
{
	FilePtr f = std::dynamic_pointer_cast<File> (get_smart());
	if (!v.visit(f))
		return false;

	return visit_children(v);
}


std::vector<Action>
File::get_actions (void)
{
	LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.file", true },
	};

	std::vector<Action> parent_actions = Container::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
File::perform_action (Action action)
{
	if (action.name == "dummy.file") {
		log_debug ("File perform: %s", action.name.c_str());
		return true;
	} else {
		return Container::perform_action (action);
	}
}


#if 0
bool
File::find_containers (const std::string& name, int fd, struct stat& st, ContainerPtr& list)
{
	FilePtr f = File::create();

	log_debug ("dev     = 0x%04lx", st.st_dev);
	log_debug ("ino     = %ld",     st.st_ino);
	log_debug ("nlink   = %ld",     st.st_nlink);
	log_debug ("mode    = %06o",    st.st_mode);
	log_debug ("uid     = %d",      st.st_uid);
	log_debug ("gid     = %d",      st.st_gid);
	log_debug ("rdev    = %ld",     st.st_rdev);
	log_debug ("size    = %ld",     st.st_size);
	log_debug ("blksize = %ld",     st.st_blksize);
	log_debug ("blocks  = %ld",     st.st_blocks);
	log_debug ("atime   = %ld",     st.st_atim.tv_sec);
	log_debug ("mtime   = %ld",     st.st_mtim.tv_sec);
	log_debug ("ctime   = %ld",     st.st_ctim.tv_sec);

	f->container        = name;
	f->parent_offset = 0;
	f->bytes_size    = st.st_size;
	f->bytes_used    = 0;

	list.add_child(f);
	queue_add_probe(f);	// queue the container for action

	return true;
}

#endif

void
File::discover (ContainerPtr& UNUSED(top_level), std::queue<ContainerPtr>& UNUSED(probe_queue))
{
	LOG_TRACE;

	// Config
	//	[files]
	//	dir = ~/vm
	//	file = /tmp/test.img
	//	suffix = img

	// iterate through config
	//	file => add that file
	//	dir  => add dir/*.img
}

void
File::identify (ContainerPtr& UNUSED(top_level), const char* UNUSED(name), int UNUSED(fd), struct stat& UNUSED(st))
{
	LOG_TRACE;

	/* dir:
	 *	Search for files: *.img
	 * file:
	 *	We create, and manage, a loop container for this file
	 */
}

