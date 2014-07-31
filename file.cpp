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

#include "action.h"
#include "log.h"
#include "visitor.h"
#include "utils.h"
#include "file.h"

File::File (void)
{
	log_ctor ("ctor File");
	const char* me = "File";

	sub_type (me);
}

File::File (const File& c) :
	Block(c)
{
	File();
	// No properties
}

File::File (File&& c)
{
	swap (c);
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


/**
 * operator= (copy)
 */
File&
File::operator= (const File& UNUSED(c))
{
	// No properties

	return *this;
}

/**
 * operator= (move)
 */
File&
File::operator= (File&& c)
{
	swap (c);
	return *this;
}


/**
 * swap (member)
 */
void
File::swap (File& UNUSED(c))
{
	// No properties
}

/**
 * swap (global)
 */
void
swap (File& lhs, File& rhs)
{
	lhs.swap (rhs);
}


File*
File::clone (void)
{
	LOG_TRACE;
	return new File (*this);
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

	std::vector<Action> parent_actions = Block::get_actions();

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
		return Block::perform_action (action);
	}
}


void
File::discover (ContainerPtr& UNUSED(parent))
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

bool
File::identify (ContainerPtr& parent, const std::string& name, int fd, struct stat& UNUSED(st))
{
	return_val_if_fail (parent, false);
	return_val_if_fail (!name.empty(), false);
	return_val_if_fail (fd>=0, false);
	LOG_TRACE;

	/* dir:
	 *	Search for files: *.img
	 * file:
	 *	We create, and manage, a loop container for this file
	 */

	return false;
}

