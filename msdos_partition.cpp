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

#include "msdos_partition.h"
#include "action.h"
#include "visitor.h"

MsdosPartition::MsdosPartition (void)
{
	LOG_CTOR;
	const char* me = "MsdosPartition";

	sub_type (me);
}

MsdosPartition::MsdosPartition (const MsdosPartition& c) :
	Partition(c)
{
	MsdosPartition();
	LOG_CTOR;
	// No properties
}

MsdosPartition::MsdosPartition (MsdosPartition&& c)
{
	LOG_CTOR;
	swap(c);
}

MsdosPartition::~MsdosPartition()
{
	LOG_DTOR;
}

MsdosPartitionPtr
MsdosPartition::create (void)
{
	MsdosPartitionPtr p (new MsdosPartition());
	p->self = p;

	return p;
}


MsdosPartition&
MsdosPartition::operator= (const MsdosPartition& UNUSED(c))
{
	// No properties

	return *this;
}

MsdosPartition&
MsdosPartition::operator= (MsdosPartition&& c)
{
	swap(c);
	return *this;
}


void
MsdosPartition::swap (MsdosPartition& UNUSED(c))
{
	// No properties
}

void
swap (MsdosPartition& lhs, MsdosPartition& rhs)
{
	lhs.swap (rhs);
}


MsdosPartition*
MsdosPartition::clone (void)
{
	LOG_TRACE;
	return new MsdosPartition (*this);
}


bool
MsdosPartition::accept (Visitor& v)
{
	MsdosPartitionPtr b = std::dynamic_pointer_cast<MsdosPartition> (get_smart());
	if (!v.visit(b))
		return false;

	return visit_children(v);
}


std::vector<Action>
MsdosPartition::get_actions (void)
{
	LOG_TRACE;

	ContainerPtr me = get_smart();
	std::vector<Action> actions = {
		{ "dummy.msdos_partition", "Dummy/Msdos Partition", me, true },
	};

	std::vector<Action> base_actions = Partition::get_actions();

	actions.insert (std::end (actions), std::begin (base_actions), std::end (base_actions));

	return actions;
}

bool
MsdosPartition::perform_action (Action action)
{
	if (action.name == "dummy.msdos_partition") {
		log_debug ("MsdosPartition perform: %s", SP(action.name));
		return true;
	} else {
		return Partition::perform_action (action);
	}
}


