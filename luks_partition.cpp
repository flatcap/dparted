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

#include "luks_partition.h"
#include "action.h"
#include "log.h"
#include "visitor.h"

LuksPartition::LuksPartition (void)
{
	log_ctor ("ctor LuksPartition");
	const char* me = "LuksPartition";

	sub_type (me);
}

LuksPartition::~LuksPartition()
{
	log_dtor ("dtor LuksPartition");
}

LuksPartitionPtr
LuksPartition::create (void)
{
	LuksPartitionPtr p (new LuksPartition());
	p->self = p;

	return p;
}


LuksPartition::LuksPartition (const LuksPartition& c) :
	Partition(c)
{
	LuksPartition();
	// No properties
}

LuksPartition::LuksPartition (LuksPartition&& c)
{
	swap (c);
}

/**
 * operator= (copy)
 */
LuksPartition&
LuksPartition::operator= (const LuksPartition& UNUSED(c))
{
	// No properties

	return *this;
}

/**
 * operator= (move)
 */
LuksPartition&
LuksPartition::operator= (LuksPartition&& c)
{
	swap (c);
	return *this;
}


/**
 * swap (member)
 */
void
LuksPartition::swap (LuksPartition& UNUSED(c))
{
	// No properties
}

/**
 * swap (global)
 */
void
swap (LuksPartition& lhs, LuksPartition& rhs)
{
	lhs.swap (rhs);
}


LuksPartition*
LuksPartition::clone (void)
{
	LOG_TRACE;
	return new LuksPartition (*this);
}


bool
LuksPartition::accept (Visitor& v)
{
	LuksPartitionPtr m = std::dynamic_pointer_cast<LuksPartition> (get_smart());
	if (!v.visit(m))
		return false;

	return visit_children(v);
}


std::vector<Action>
LuksPartition::get_actions (void)
{
	LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.luks_partition", true },
	};

	std::vector<Action> parent_actions = Partition::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
LuksPartition::perform_action (Action action)
{
	if (action.name == "dummy.luks_partition") {
		log_debug ("LuksPartition perform: %s", action.name.c_str());
		return true;
	} else {
		return Partition::perform_action (action);
	}
}


