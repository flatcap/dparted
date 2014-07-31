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
	log_ctor ("ctor MdPartition");
	const char* me = "MdPartition";

	sub_type (me);
}

MdPartition::MdPartition (const MdPartition& UNUSED(c)) :
	MdPartition()
{
	// No properties
}

MdPartition::MdPartition (MdPartition&& c)
{
	swap (c);
}

MdPartition::~MdPartition()
{
	log_dtor ("dtor MdPartition");
}

MdPartitionPtr
MdPartition::create (void)
{
	MdPartitionPtr p (new MdPartition());
	p->self = p;

	return p;
}


/**
 * operator= (copy)
 */
MdPartition&
MdPartition::operator= (const MdPartition& UNUSED(c))
{
	// No properties

	return *this;
}

/**
 * operator= (move)
 */
MdPartition&
MdPartition::operator= (MdPartition&& c)
{
	swap (c);
	return *this;
}


/**
 * swap (member)
 */
void
MdPartition::swap (MdPartition& UNUSED(c))
{
	// No properties
}

/**
 * swap (global)
 */
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
	std::vector<Action> actions = {
		{ "dummy.md_partition", true },
	};

	std::vector<Action> parent_actions = Partition::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
MdPartition::perform_action (Action action)
{
	if (action.name == "dummy.md_partition") {
		log_debug ("MdPartition perform: %s", action.name.c_str());
		return true;
	} else {
		return Partition::perform_action (action);
	}
}


