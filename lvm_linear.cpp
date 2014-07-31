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
#include "action.h"
#include "log.h"
#include "visitor.h"

LvmLinear::LvmLinear (void)
{
	log_ctor ("ctor LvmLinear");
	const char* me = "LvmLinear";

	sub_type (me);
}

LvmLinear::LvmLinear (const LvmLinear& UNUSED(c)) :
	LvmLinear()
{
	// No properties
}

LvmLinear::LvmLinear (LvmLinear&& c)
{
	swap (c);
}

LvmLinear::~LvmLinear()
{
	log_dtor ("dtor LvmLinear");
}

LvmLinearPtr
LvmLinear::create (void)
{
	LvmLinearPtr p (new LvmLinear());
	p->self = p;

	return p;
}


/**
 * operator= (copy)
 */
LvmLinear&
LvmLinear::operator= (const LvmLinear& UNUSED(c))
{
	// No properties

	return *this;
}

/**
 * operator= (move)
 */
LvmLinear&
LvmLinear::operator= (LvmLinear&& c)
{
	swap (c);
	return *this;
}


/**
 * swap (member)
 */
void
LvmLinear::swap (LvmLinear& UNUSED(c))
{
	// No properties
}

/**
 * swap (global)
 */
void
swap (LvmLinear& lhs, LvmLinear& rhs)
{
	lhs.swap (rhs);
}


LvmLinear*
LvmLinear::clone (void)
{
	LOG_TRACE;
	return new LvmLinear (*this);
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
	LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.lvm_linear", true },
	};

	std::vector<Action> parent_actions = LvmVolume::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
LvmLinear::perform_action (Action action)
{
	if (action.name == "dummy.lvm_linear") {
		log_debug ("LvmLinear perform: %s", action.name.c_str());
		return true;
	} else {
		return LvmVolume::perform_action (action);
	}
}

