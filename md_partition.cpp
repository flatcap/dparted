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

#include <sstream>

#include "md_partition.h"
#include "action.h"
#include "log_trace.h"
#include "visitor.h"

MdPartition::MdPartition (void)
{
	const char* me = "MdPartition";

	sub_type (me);
}

MdPartition::~MdPartition()
{
}

MdPartitionPtr
MdPartition::create (void)
{
	MdPartitionPtr p (new MdPartition());
	p->weak = p;

	return p;
}


bool
MdPartition::accept (Visitor& v)
{
	MdPartitionPtr m = std::dynamic_pointer_cast<MdPartition> (get_smart());
	if (!v.visit(m))
		return false;
	return visit_children(v);
}


std::vector<Action>
MdPartition::get_actions (void)
{
	 //LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.md_partition", true },
	};

	std::vector<Action> parent_actions = Partition::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
MdPartition::perform_action (Action action)
{
	if (action.name == "dummy.md_partition") {
		log_debug ("MdPartition perform: %s\n", action.name.c_str());
		return true;
	} else {
		return Partition::perform_action (action);
	}
}


