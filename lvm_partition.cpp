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
#include "action.h"
#include "log.h"
#include "visitor.h"

LvmPartition::LvmPartition (void)
{
	log_ctor ("ctor LvmPartition");
	const char* me = "LvmPartition";

	sub_type (me);
}

LvmPartition::LvmPartition (const LvmPartition& UNUSED(c)) :
	LvmPartition()
{
	// No properties
}

LvmPartition::LvmPartition (LvmPartition&& c)
{
	swap (c);
}

LvmPartition::~LvmPartition()
{
	log_dtor ("dtor LvmPartition");
}

LvmPartitionPtr
LvmPartition::create (void)
{
	LvmPartitionPtr p (new LvmPartition());
	p->self = p;

	return p;
}


/**
 * operator= (copy)
 */
LvmPartition&
LvmPartition::operator= (const LvmPartition& UNUSED(c))
{
	// No properties

	return *this;
}

/**
 * operator= (move)
 */
LvmPartition&
LvmPartition::operator= (LvmPartition&& c)
{
	swap (c);
	return *this;
}


/**
 * swap (member)
 */
void
LvmPartition::swap (LvmPartition& UNUSED(c))
{
	// No properties
}

/**
 * swap (global)
 */
void
swap (LvmPartition& lhs, LvmPartition& rhs)
{
	lhs.swap (rhs);
}


LvmPartition*
LvmPartition::clone (void)
{
	LOG_TRACE;
	return new LvmPartition (*this);
}


bool
LvmPartition::accept (Visitor& v)
{
	LvmPartitionPtr l = std::dynamic_pointer_cast<LvmPartition> (get_smart());
	if (!v.visit(l))
		return false;

	return visit_children(v);
}


std::vector<Action>
LvmPartition::get_actions (void)
{
	LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.lvm_partition", true },
	};

	std::vector<Action> parent_actions = Partition::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
LvmPartition::perform_action (Action action)
{
	if (action.name == "dummy.lvm_partition") {
		log_debug ("LvmPartition perform: %s", action.name.c_str());
		return true;
	} else {
		return Partition::perform_action (action);
	}
}


