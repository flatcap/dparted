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

#include "lvm_partition.h"
#include "log_trace.h"
#include "visitor.h"

/**
 * LvmPartition
 */
LvmPartition::LvmPartition (void)
{
	const char* me = "LvmPartition";

	sub_type (me);
}

/**
 * create
 */
LvmPartitionPtr
LvmPartition::create (void)
{
	LvmPartitionPtr l (new LvmPartition());
	l->weak = l;

	return l;
}


/**
 * accept
 */
bool
LvmPartition::accept (Visitor& v)
{
	LvmPartitionPtr l = std::dynamic_pointer_cast<LvmPartition> (get_smart());
	if (!v.visit(l))
		return false;
	return visit_children(v);
}


/**
 * get_actions
 */
std::vector<Action>
LvmPartition::get_actions (void)
{
	// LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.lvmpartition", true },
	};

	std::vector<Action> parent_actions = Piece::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

/**
 * perform_action
 */
bool
LvmPartition::perform_action (Action action)
{
	if (action.name == "dummy.lvmpartition") {
		std::cout << "LvmPartition perform: " << action.name << std::endl;
		return true;
	} else {
		return Piece::perform_action (action);
	}
}


