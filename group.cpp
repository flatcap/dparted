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

#include "group.h"
#include "action.h"
#include "visitor.h"

Group::Group (void)
{
}

Group::~Group()
{
}

GroupPtr
Group::create (void)
{
	GroupPtr p (new Group());
	p->weak = p;

	return p;
}


bool
Group::accept (Visitor& v)
{
	GroupPtr b = std::dynamic_pointer_cast<Group> (get_smart());
	if (!v.visit(b))
		return false;
	return visit_children(v);
}


std::vector<Action>
Group::get_actions (void)
{
	// LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.group", true },
	};

	std::vector<Action> parent_actions = Whole::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
Group::perform_action (Action action)
{
	if (action.name == "dummy.group") {
		std::cout << "Group perform: " << action.name << std::endl;
		return true;
	} else {
		return Whole::perform_action (action);
	}
}


