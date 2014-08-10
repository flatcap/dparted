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
#include <string>

#include "partition.h"
#include "action.h"
#include "log.h"
#include "visitor.h"

Partition::Partition (void)
{
	LOG_CTOR;
	const char* me = "Partition";

	sub_type (me);

	declare_prop_var (me, "ptype", ptype, "desc of ptype", 0);
}

Partition::Partition (const Partition& c) :
	Container(c)
{
	Partition();
	LOG_CTOR;
	ptype  = c.ptype;
	volume = c.volume;
}

Partition::Partition (Partition&& c)
{
	LOG_CTOR;
	swap(c);
}

Partition::~Partition()
{
	LOG_DTOR;
}

PartitionPtr
Partition::create (void)
{
	PartitionPtr p (new Partition());
	p->self = p;

	return p;
}


Partition&
Partition::operator= (const Partition& c)
{
	ptype  = c.ptype;
	volume = c.volume;

	return *this;
}

Partition&
Partition::operator= (Partition&& c)
{
	swap(c);
	return *this;
}


void
Partition::swap (Partition& c)
{
	std::swap (ptype,  c.ptype);
	std::swap (volume, c.volume);
}

void
swap (Partition& lhs, Partition& rhs)
{
	lhs.swap (rhs);
}


Partition*
Partition::clone (void)
{
	return new Partition (*this);
}


bool
Partition::accept (Visitor& v)
{
	PartitionPtr p = std::dynamic_pointer_cast<Partition> (get_smart());
	if (!v.visit(p))
		return false;

	return visit_children(v);
}


std::vector<Action>
Partition::get_actions (void)
{
	LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.partition", true },
	};

	std::vector<Action> parent_actions = Container::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
Partition::perform_action (Action action)
{
	if (action.name == "dummy.partition") {
		log_debug ("Partition perform: %s", action.name.c_str());
		return true;
	} else {
		return Container::perform_action (action);
	}
}


void
Partition::add_child (ContainerPtr& child, bool probe)
{
	return_if_fail (child);

	Container::add_child (child, probe);

	if ((children.size() == 1) && (get_bytes_free() != 0)) {
		PartitionPtr p = Partition::create();
		p->sub_type ("Space");
		p->sub_type ("Unreachable");
		p->bytes_size = get_bytes_free();
		p->bytes_used = p->bytes_size;
		p->parent_offset = this->bytes_size - p->bytes_size;	// End of the device
		Container::add_child (p, false);
	}
}


