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

#include "md_raid.h"
#include "action.h"
#include "visitor.h"

MdRaid::MdRaid (void)
{
	const char* me = "MdRaid";

	sub_type (me);
}

MdRaid::~MdRaid()
{
}

MdRaidPtr
MdRaid::create (void)
{
	MdRaidPtr p (new MdRaid());
	p->self = p;

	return p;
}


bool
MdRaid::accept (Visitor& v)
{
	MdRaidPtr b = std::dynamic_pointer_cast<MdRaid> (get_smart());
	if (!v.visit(b))
		return false;

	return visit_children(v);
}


std::vector<Action>
MdRaid::get_actions (void)
{
	// LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.md_raid", true },
	};

	std::vector<Action> parent_actions = MdVolume::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
MdRaid::perform_action (Action action)
{
	if (action.name == "dummy.md_raid") {
		log_debug ("MdRaid perform: %s\n", action.name.c_str());
		return true;
	} else {
		return MdVolume::perform_action (action);
	}
}


