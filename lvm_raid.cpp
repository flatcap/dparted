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

#include "lvm_raid.h"
#include "action.h"
#include "log.h"
#include "log_trace.h"
#include "visitor.h"

LvmRaid::LvmRaid (void)
{
	const char* me = "LvmRaid";

	sub_type (me);
}

LvmRaid::~LvmRaid()
{
}

LvmRaidPtr
LvmRaid::create (void)
{
	LvmRaidPtr p (new LvmRaid());
	p->self = p;

	return p;
}


bool
LvmRaid::accept (Visitor& v)
{
	LvmRaidPtr l = std::dynamic_pointer_cast<LvmRaid> (get_smart());
	if (!v.visit(l))
		return false;

	//XXX this needs to be in LvmVolume
	ContainerPtr c = get_smart();
	if (!v.visit_enter(c))
		return false;

	for (auto m : metadata) {
		if (!m->accept(v))
			return false;
	}

	for (auto s : subvols) {
		if (!s->accept(v))
			return false;
	}

	if (!v.visit_leave())
		return false;

	return visit_children(v);
}


std::vector<Action>
LvmRaid::get_actions (void)
{
	LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.lvm_raid", true },
	};

	std::vector<Action> parent_actions = LvmVolume::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
LvmRaid::perform_action (Action action)
{
	if (action.name == "dummy.lvm_raid") {
		log_debug ("LvmRaid perform: %s\n", action.name.c_str());
		return true;
	} else {
		return LvmVolume::perform_action (action);
	}
}


