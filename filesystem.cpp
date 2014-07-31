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
#ifdef DP_BTRFS
#include "btrfs.h"
#endif
#ifdef DP_EXTFS
#include "extfs.h"
#endif
#ifdef DP_FS_MISC
#include "fs_get.h"
#include "fs_identify.h"
#endif
#include "log.h"
#ifdef DP_NTFS
#include "ntfs.h"
#endif
#include "partition.h"
#include "question.h"
#include "stringnum.h"
#include "utils.h"
#include "visitor.h"

Filesystem::Filesystem (void)
{
	LOG_CTOR;
	const char* me = "Filesystem";

	sub_type (me);
}

Filesystem::Filesystem (const Filesystem& c) :
	Container(c)
{
	Filesystem();
	LOG_CTOR;
	// No properties
}

Filesystem::Filesystem (Filesystem&& c)
{
	LOG_CTOR;
	swap (c);
}

Filesystem::~Filesystem()
{
	LOG_DTOR;
}

FilesystemPtr
Filesystem::create (void)
{
	FilesystemPtr p (new Filesystem());
	p->self = p;

	return p;
}


/**
 * operator= (copy)
 */
Filesystem&
Filesystem::operator= (const Filesystem& UNUSED(c))
{
	// No properties

	return *this;
}

/**
 * operator= (move)
 */
Filesystem&
Filesystem::operator= (Filesystem&& c)
{
	swap (c);
	return *this;
}


/**
 * swap (member)
 */
void
Filesystem::swap (Filesystem& UNUSED(c))
{
	// No properties
}

/**
 * swap (global)
 */
void
swap (Filesystem& lhs, Filesystem& rhs)
{
	lhs.swap (rhs);
}


Filesystem*
Filesystem::clone (void)
{
	LOG_TRACE;
	return new Filesystem (*this);
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
	LOG_TRACE;
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
		log_debug ("Filesystem perform: %s", action.name.c_str());
		return true;
	} else {
		return Container::perform_action (action);
	}
}


bool
Filesystem::probe (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize)
{
	return_val_if_fail (parent,  false);
	return_val_if_fail (buffer,  false);
	return_val_if_fail (bufsize, false);
	LOG_TRACE;

	FilesystemPtr f;

	//XXX reorder by likelihood
	if (false) {}
#ifdef DP_BTRFS
	else if ((f = Btrfs::get_btrfs    (parent, buffer, bufsize))) {}
#endif
#ifdef DP_EXTFS
	else if ((f = Extfs::get_ext2     (parent, buffer, bufsize))) {}
	else if ((f = Extfs::get_ext3     (parent, buffer, bufsize))) {}
	else if ((f = Extfs::get_ext4     (parent, buffer, bufsize))) {}
#endif
#ifdef DP_NTFS
	else if ((f =  Ntfs::get_ntfs     (parent, buffer, bufsize))) {}
#endif
#ifdef DP_FS_MISC
	else if ((f =        get_reiserfs (parent, buffer, bufsize))) {}
	else if ((f =        get_swap     (parent, buffer, bufsize))) {}
	else if ((f =        get_vfat     (parent, buffer, bufsize))) {}
	else if ((f =        get_xfs      (parent, buffer, bufsize))) {}
#endif

	if (f) {
		log_debug ("volume: %s (%s), child: %s", parent->name.c_str(), parent->get_type().c_str(), f->name.c_str());
		std::string desc = "Discovered " + f->get_type() + " filesystem";
		parent->add_child (f, false, desc.c_str());	//XXX assumption fs is a leaf
						//XXX move this into get_*?
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
	LOG_TRACE;

	Question q { "Delete filesystem",
		     "Are you sure?",
		     { "Yes", "No" } };

	main_app->ask(q);
}

#endif

