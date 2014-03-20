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

#include <cstdlib>
#include <sstream>
#include <string>

#include "filesystem.h"
#include "action.h"
#include "app.h"
#include "btrfs.h"
#include "extfs.h"
#ifdef DP_FS_MISC
#include "fs_get.h"
#include "fs_identify.h"
#endif
#include "log.h"
#include "log_trace.h"
#include "main.h"
#include "ntfs.h"
#include "partition.h"
#include "question.h"
#include "stringnum.h"
#include "utils.h"
#include "visitor.h"

Filesystem::Filesystem (void)
{
	const char* me = "Filesystem";

	sub_type (me);
}

Filesystem::~Filesystem()
{
}

FilesystemPtr
Filesystem::create (void)
{
	FilesystemPtr p (new Filesystem());
	p->weak = p;

	return p;
}


bool
Filesystem::accept (Visitor& v)
{
	FilesystemPtr f = std::dynamic_pointer_cast<Filesystem> (get_smart());
	if (!v.visit(f))
		return false;
	return visit_children(v);
}


std::vector<Action>
Filesystem::get_actions (void)
{
	// LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.filesystem", true },
	};

	std::vector<Action> parent_actions = Container::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
Filesystem::perform_action (Action action)
{
	if (action.name == "dummy.filesystem") {
		std::cout << "Filesystem perform: " << action.name << std::endl;
		return true;
	} else {
		return Container::perform_action (action);
	}
}


bool
Filesystem::probe (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize)
{
	//LOG_TRACE;

	if (!parent || !buffer || !bufsize)
		return false;

	FilesystemPtr f;

	//XXX reorder by likelihood
	     if ((f = Btrfs::get_btrfs    (parent, buffer, bufsize))) {}
	else if ((f = Extfs::get_ext2     (parent, buffer, bufsize))) {}
	else if ((f = Extfs::get_ext3     (parent, buffer, bufsize))) {}
	else if ((f = Extfs::get_ext4     (parent, buffer, bufsize))) {}
	else if ((f =  Ntfs::get_ntfs     (parent, buffer, bufsize))) {}
#ifdef DP_FS_MISC
	else if ((f =        get_reiserfs (parent, buffer, bufsize))) {}
	else if ((f =        get_swap     (parent, buffer, bufsize))) {}
	else if ((f =        get_vfat     (parent, buffer, bufsize))) {}
	else if ((f =        get_xfs      (parent, buffer, bufsize))) {}
#endif

	if (f) {
		//log_info ("volume: %s (%s), child: %s\n", parent->name.c_str(), parent->type.back().c_str(), f->name.c_str());
		parent->add_child(f);	//XXX move this into get_*?
		return true;
	}

	return false;
}

bool
Filesystem::get_mounted_usage (ContainerPtr UNUSED(parent))
{
	return false;
}


#if 0
void
Filesystem::delete_filesystem (void)
{
	std::cout << __PRETTY_FUNCTION__ << std::endl;

	Question q { "Delete filesystem",
		     "Are you sure?",
		     { "Yes", "No" } };

	main_app->ask(q);
}

#endif

