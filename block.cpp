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
#include "action.h"
#include "disk.h"
#include "file.h"
#include "log.h"
#include "log_trace.h"
#include "loop.h"
#include "main.h"
#include "visitor.h"
#include "log.h"

Block::Block (void)
{
	const char* me = "Block";

	sub_type (me);
}

Block::~Block()
{
}

BlockPtr
Block::create (void)
{
	BlockPtr p (new Block());
	p->weak = p;

	return p;
}


bool
Block::accept (Visitor& v)
{
	BlockPtr b = std::dynamic_pointer_cast<Block> (get_smart());
	if (!v.visit(b))
		return false;
	return visit_children(v);
}


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

bool
Block::perform_action (Action action)
{
	if (action.name == "dummy.block") {
		log_debug ("Block perform: %s\n", action.name.c_str());
		return true;
	} else {
		return Container::perform_action (action);
	}
}


