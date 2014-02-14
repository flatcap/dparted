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
#include <string>

#include "log.h"
#include "partition.h"
#include "log_trace.h"
#include "visitor.h"

/**
 * Partition
 */
Partition::Partition (void)
{
	const char* me = "Partition";

	sub_type (me);

	declare_prop (me, "ptype", ptype, "desc of ptype");
}

/**
 * create
 */
PartitionPtr
Partition::create (void)
{
	PartitionPtr p (new Partition());
	p->weak = p;

	return p;
}


/**
 * accept
 */
bool
Partition::accept (Visitor& v)
{
	PartitionPtr p = std::dynamic_pointer_cast<Partition> (get_smart());
	if (!v.visit(p))
		return false;
	return visit_children(v);
}

/**
 * get_actions
 */
std::vector<Action>
Partition::get_actions (void)
{
	// LOG_TRACE;
	std::vector<Action> actions = {
		//{ "create.filesystem", true },
	};

	std::vector<Action> parent_actions = Container::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

/**
 * perform_action
 */
bool
Partition::perform_action (Action action)
{
	if (action.name == "create.table") {
		std::cout << "Partition perform: " << action.name << std::endl;
		return true;
	} else {
		std::cout << "Unknown action: " << action.name << std::endl;
		return false;
	}
}


