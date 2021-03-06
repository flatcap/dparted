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

#include "md_partition.h"
#include "action.h"
#include "log.h"
#include "visitor.h"

MdPartition::MdPartition (void)
{
	LOG_CTOR;
	const char* me = "MdPartition";

	sub_type (me);
}

MdPartition::MdPartition (const MdPartition& c) :
	Partition(c)
{
	MdPartition();
	LOG_CTOR;
	// No properties
}

MdPartition::MdPartition (MdPartition&& c)
{
	LOG_CTOR;
	swap(c);
}

MdPartition::~MdPartition()
{
	LOG_DTOR;
}

MdPartitionPtr
MdPartition::create (void)
{
	MdPartitionPtr p (new MdPartition());
	p->self = p;

	return p;
}


MdPartition&
MdPartition::operator= (const MdPartition& UNUSED(c))
{
	// No properties

	return *this;
}

MdPartition&
MdPartition::operator= (MdPartition&& c)
{
	swap(c);
	return *this;
}


void
MdPartition::swap (MdPartition& UNUSED(c))
{
	// No properties
}

void
swap (MdPartition& lhs, MdPartition& rhs)
{
	lhs.swap (rhs);
}


MdPartition*
MdPartition::clone (void)
{
	LOG_TRACE;
	return new MdPartition (*this);
}


bool
MdPartition::accept (Visitor& v)
{
	MdPartitionPtr m = std::dynamic_pointer_cast<MdPartition> (get_smart());
	if (!v.visit(m))
		return false;

	return visit_children(v);
}


std::vector<Action>
MdPartition::get_actions (void)
{
	LOG_TRACE;

	ContainerPtr me = get_smart();
	std::vector<Action> actions = {
		{ "dummy.md_partition", "Dummy/MD Partition", me, true },
	};

	std::vector<Action> base_actions = Partition::get_actions();

	actions.insert (std::end (actions), std::begin (base_actions), std::end (base_actions));

	return actions;
}

bool
MdPartition::perform_action (Action action)
{
	if (action.name == "dummy.md_partition") {
		log_debug ("MdPartition perform: %s", SP(action.name));
		return true;
	} else {
		return Partition::perform_action (action);
	}
}


