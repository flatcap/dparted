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

#include "gpt_partition.h"
#include "action.h"
#include "visitor.h"

GptPartition::GptPartition (void)
{
	LOG_CTOR;
	const char* me = "GptPartition";

	sub_type (me);
}

GptPartition::GptPartition (const GptPartition& c) :
	Partition(c)
{
	GptPartition();
	LOG_CTOR;
	// No properties
}

GptPartition::GptPartition (GptPartition&& c)
{
	LOG_CTOR;
	swap(c);
}

GptPartition::~GptPartition()
{
	LOG_DTOR;
}

GptPartitionPtr
GptPartition::create (void)
{
	GptPartitionPtr p (new GptPartition());
	p->self = p;

	return p;
}


GptPartition&
GptPartition::operator= (const GptPartition& UNUSED(c))
{
	// No properties

	return *this;
}

GptPartition&
GptPartition::operator= (GptPartition&& c)
{
	swap(c);
	return *this;
}


void
GptPartition::swap (GptPartition& UNUSED(c))
{
	// No properties
}

void
swap (GptPartition& lhs, GptPartition& rhs)
{
	lhs.swap (rhs);
}


GptPartition*
GptPartition::clone (void)
{
	LOG_TRACE;
	return new GptPartition (*this);
}


bool
GptPartition::accept (Visitor& v)
{
	GptPartitionPtr b = std::dynamic_pointer_cast<GptPartition> (get_smart());
	if (!v.visit(b))
		return false;

	return visit_children(v);
}


std::vector<Action>
GptPartition::get_actions (void)
{
	LOG_TRACE;
	std::vector<Action> actions = {
		//{ "dummy.gpt_partition", true },
	};

	std::vector<Action> base_actions = Partition::get_actions();

	actions.insert (std::end (actions), std::begin (base_actions), std::end (base_actions));

	return actions;
}

bool
GptPartition::perform_action (Action action)
{
	// Currently no actions to check

	return Partition::perform_action (action);
}


