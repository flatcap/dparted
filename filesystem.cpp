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

#include <cstdlib>
#include <sstream>
#include <string>

#include "filesystem.h"
#include "fs_identify.h"
#include "fs_get.h"
#include "log.h"
#include "partition.h"
#include "stringnum.h"
#include "utils.h"
#include "log_trace.h"
#include "visitor.h"
#include "question.h"
#include "main.h"
#include "app.h"

/**
 * Filesystem
 */
Filesystem::Filesystem (void)
{
	declare ("filesystem");
}

/**
 * create
 */
FilesystemPtr
Filesystem::create (void)
{
	FilesystemPtr f (new Filesystem());
	f->weak = f;

	return f;
}


/**
 * accept
 */
bool
Filesystem::accept (Visitor& v)
{
	FilesystemPtr f = std::dynamic_pointer_cast<Filesystem> (get_smart());
	if (!v.visit (f))
		return false;
	return visit_children (v);
}


/**
 * probe
 */
FilesystemPtr
Filesystem::probe (ContainerPtr& top_level, ContainerPtr& parent)
{
	//LOG_TRACE;

	if (!parent)
		return nullptr;

	ContainerPtr p (parent);
	while (p) {
		////std::cout << p << std::endl;
		p = p->parent.lock();
	}

	long		 bufsize = 131072;	// 128 KiB, enough for the fs signatures
	unsigned char	*buffer  = parent->get_buffer (0, bufsize);

	if (!buffer) {
		//log_error ("can't get buffer\n");
		return nullptr;
	}

	FilesystemPtr f;

	//XXX reorder by likelihood
	     if ((f = get_btrfs    (buffer, bufsize))) {}
	else if ((f = get_ext2     (buffer, bufsize))) {}
	else if ((f = get_ext3     (buffer, bufsize))) {}
	else if ((f = get_ext4     (buffer, bufsize))) {}
	else if ((f = get_ntfs     (buffer, bufsize))) {}
	else if ((f = get_reiserfs (buffer, bufsize))) {}
	else if ((f = get_swap     (buffer, bufsize))) {}
	else if ((f = get_vfat     (buffer, bufsize))) {}
	else if ((f = get_xfs      (buffer, bufsize))) {}

	if (f) {
		//log_info ("volume: %s (%s), child: %s\n", parent->name.c_str(), parent->type.back().c_str(), f->name.c_str());
		ContainerPtr c(f);
		parent->add_child (c);
	}

	return f;
}


/**
 * get_property
 */
std::string
Filesystem::get_property (const std::string& propname)
{
	if (propname == "label") {
		return label;
	} else {
		return Container::get_property (propname);
	}
}


/**
 * mouse_event
 */
void
Filesystem::mouse_event (void)
{
	std::cout << __PRETTY_FUNCTION__ << std::endl;

	Question q { "Delete filesystem",
		     "Are you sure?",
		     { "Yes", "No" } };

	main_app->ask(q);
}

