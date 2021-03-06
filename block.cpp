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
#include "log.h"
#ifdef DP_DISK
#include "disk.h"
#endif
#ifdef DP_FILE
#include "file.h"
#endif
#ifdef DP_LOOP
#include "loop.h"
#endif
#include "visitor.h"

Block::Block (void)
{
	LOG_CTOR;
	const char* me = "Block";

	sub_type (me);
}

Block::Block (const Block& c) :
	Container(c)
{
	Block();
	LOG_CTOR;
	// No properties
}

Block::Block (Block&& c)
{
	LOG_CTOR;
	swap(c);
}

Block::~Block()
{
	LOG_DTOR;
}

BlockPtr
Block::create (void)
{
	BlockPtr p (new Block());
	p->self = p;

	return p;
}


Block&
Block::operator= (const Block& UNUSED(c))
{
	// No properties

	return *this;
}

Block&
Block::operator= (Block&& c)
{
	swap(c);
	return *this;
}


void
Block::swap (Block& UNUSED(c))
{
	// No properties
}

void
swap (Block& lhs, Block& rhs)
{
	lhs.swap (rhs);
}


Block*
Block::clone (void)
{
	LOG_TRACE;
	return new Block (*this);
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
	LOG_TRACE;

	ContainerPtr me = get_smart();
	std::vector<Action> actions = {
		{ "block.readonly", "Make device read-only", me, true }
	};

	std::vector<Action> base_actions = Container::get_actions();

	actions.insert (std::end (actions), std::begin (base_actions), std::end (base_actions));

	// if ((children.size() == 1) && (!children[0]->is_a ("Table"))) {
	// 	actions.push_back ({"CREATE TABLE", true});
	// }

	return actions;
}

bool
Block::perform_action (Action action)
{
	if (action.name == "block.readonly") {
		log_error ("Block perform: %s", SP(action.name));
		return true;
	} else {
		return Container::perform_action (action);
	}
}

bool
Block::is_resizeable (void)
{
	return false;
}

