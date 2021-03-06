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
	LOG_CTOR;
	const char* me = "LuksPartition";

	sub_type (me);
}

LuksPartition::~LuksPartition()
{
	LOG_DTOR;
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
	LOG_CTOR;
	// No properties
}

LuksPartition::LuksPartition (LuksPartition&& c)
{
	LOG_CTOR;
	swap(c);
}

LuksPartition&
LuksPartition::operator= (const LuksPartition& UNUSED(c))
{
	// No properties

	return *this;
}

LuksPartition&
LuksPartition::operator= (LuksPartition&& c)
{
	swap(c);
	return *this;
}


void
LuksPartition::swap (LuksPartition& UNUSED(c))
{
	// No properties
}

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

	ContainerPtr me = get_smart();
	std::vector<Action> actions = {
		{ "dummy.luks_partition", "Dummy/Luks Partition", me, true },
	};

	std::vector<Action> base_actions = Partition::get_actions();

	actions.insert (std::end (actions), std::begin (base_actions), std::end (base_actions));

	return actions;
}

bool
LuksPartition::perform_action (Action action)
{
	if (action.name == "dummy.luks_partition") {
		log_debug ("LuksPartition perform: %s", SP(action.name));
		return true;
	} else {
		return Partition::perform_action (action);
	}
}


