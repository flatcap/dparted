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

#include "lvm_linear.h"
#include "log_trace.h"
#include "visitor.h"

LvmLinear::LvmLinear (void)
{
	const char* me = "LvmLinear";

	sub_type (me);
}

LvmLinearPtr
LvmLinear::create (void)
{
	LvmLinearPtr l (new LvmLinear());
	l->weak = l;

	return l;
}


bool
LvmLinear::accept (Visitor& v)
{
	LvmLinearPtr l = std::dynamic_pointer_cast<LvmLinear> (get_smart());
	if (!v.visit(l))
		return false;
	return visit_children(v);
}


std::vector<Action>
LvmLinear::get_actions (void)
{
	// LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.lvmlinear", true },
	};

	std::vector<Action> parent_actions = LvmVolume::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
LvmLinear::perform_action (Action action)
{
	if (action.name == "dummy.lvmlinear") {
		std::cout << "LvmLinear perform: " << action.name << std::endl;
		return true;
	} else {
		return LvmVolume::perform_action (action);
	}
}


