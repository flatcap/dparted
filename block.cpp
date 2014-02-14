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
#include <linux/kdev_t.h>
#include <linux/major.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <sstream>
#include <string>

#include "block.h"
#include "disk.h"
#include "file.h"
#include "log.h"
#include "loop.h"
#include "main.h"
#include "log_trace.h"
#include "visitor.h"

/**
 * Block
 */
Block::Block (void)
{
	const char* me = "Block";

	sub_type (me);

	declare_prop (me, "kernel_major", kernel_major, "desc of kernel_major");
	declare_prop (me, "kernel_minor", kernel_minor, "desc of kernel_minor");
}

/**
 * create
 */
BlockPtr
Block::create (void)
{
	BlockPtr b (new Block());
	b->weak = b;

	return b;
}


/**
 * accept
 */
bool
Block::accept (Visitor& v)
{
	BlockPtr b = std::dynamic_pointer_cast<Block> (get_smart());
	if (!v.visit(b))
		return false;
	return visit_children(v);
}


/**
 * get_actions
 */
std::vector<Action>
Block::get_actions (void)
{
	// LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.block", true },
	};

	std::vector<Action> parent_actions = Container::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

/**
 * perform_action
 */
bool
Block::perform_action (Action action)
{
	if (action.name == "dummy.block") {
		std::cout << "Block perform: " << action.name << std::endl;
		return true;
	} else {
		return Container::perform_action (action);
	}
}


