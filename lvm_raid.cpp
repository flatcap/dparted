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
#include "visitor.h"

LvmRaid::LvmRaid (void)
{
	LOG_CTOR;
	const char* me = "LvmRaid";

	sub_type (me);
}

LvmRaid::LvmRaid (const LvmRaid& c) :
	LvmVolume(c)
{
	LvmRaid();
	LOG_CTOR;
	// No properties
}

LvmRaid::LvmRaid (LvmRaid&& c)
{
	LOG_CTOR;
	swap(c);
}

LvmRaid::~LvmRaid()
{
	LOG_DTOR;
}

LvmRaidPtr
LvmRaid::create (void)
{
	LvmRaidPtr p (new LvmRaid());
	p->self = p;

	return p;
}


LvmRaid&
LvmRaid::operator= (const LvmRaid& UNUSED(c))
{
	// No properties

	return *this;
}

LvmRaid&
LvmRaid::operator= (LvmRaid&& c)
{
	swap(c);
	return *this;
}


void
LvmRaid::swap (LvmRaid& UNUSED(c))
{
	// No properties
}

void
swap (LvmRaid& lhs, LvmRaid& rhs)
{
	lhs.swap (rhs);
}


LvmRaid*
LvmRaid::clone (void)
{
	LOG_TRACE;
	return new LvmRaid (*this);
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

	for (auto& m : metadata) {
		if (!m->accept(v)) {
			return false;
		}
	}

	for (auto& s : subvols) {
		if (!s->accept(v)) {
			return false;
		}
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

	std::vector<Action> base_actions = LvmVolume::get_actions();

	actions.insert (std::end (actions), std::begin (base_actions), std::end (base_actions));

	return actions;
}

bool
LvmRaid::perform_action (Action action)
{
	if (action.name == "dummy.lvm_raid") {
		log_debug ("LvmRaid perform: %s", SP(action.name));
		return true;
	} else {
		return LvmVolume::perform_action (action);
	}
}


