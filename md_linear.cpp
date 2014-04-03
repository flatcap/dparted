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

#include "md_linear.h"
#include "action.h"
#include "visitor.h"

MdLinear::MdLinear (void)
{
	const char* me = "MdLinear";

	sub_type (me);
}

MdLinear::~MdLinear()
{
}

MdLinearPtr
MdLinear::create (void)
{
	MdLinearPtr p (new MdLinear());
	p->self = p;

	return p;
}


bool
MdLinear::accept (Visitor& v)
{
	MdLinearPtr b = std::dynamic_pointer_cast<MdLinear> (get_smart());
	if (!v.visit(b))
		return false;

	return visit_children(v);
}


std::vector<Action>
MdLinear::get_actions (void)
{
	// LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.md_linear", true },
	};

	std::vector<Action> parent_actions = MdVolume::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
MdLinear::perform_action (Action action)
{
	if (action.name == "dummy.md_linear") {
		log_debug ("MdLinear perform: %s\n", action.name.c_str());
		return true;
	} else {
		return MdVolume::perform_action (action);
	}
}


