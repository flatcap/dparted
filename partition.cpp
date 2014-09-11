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
		{ "delete.partition", true },
		{ "resize.partition", true },
	};

	std::vector<Action> base_actions = Container::get_actions();

	actions.insert (std::end (actions), std::begin (base_actions), std::end (base_actions));

	if (is_a ("Unallocated")) {
		// Delegate to out parent object
		ContainerPtr parent = get_parent();
		if (parent) {
			std::vector<Action> base_actions = parent->get_actions();

			actions.insert (std::end (actions), std::begin (base_actions), std::end (base_actions));
		}
	}

	return actions;
}

bool
Partition::perform_action (Action action)
{
	if (action.name == "delete.partition") {
		log_error ("REALLY delete partition");
		return true;
	} else {
		return Container::perform_action (action);
	}
}


bool
Partition::add_child (ContainerPtr child, bool probe)
{
	return_val_if_fail (child, false);

	if (is_a ("Unallocated")) {
		return get_parent()->add_child (child, probe);		//XXX redirect to our parent
	}

	if (!Container::add_child (child, probe))
		return false;

	if ((children.size() == 1) && (get_bytes_free() != 0)) {
		PartitionPtr p = Partition::create();
		p->sub_type ("Space");
		p->sub_type ("Unreachable");
		p->bytes_size = get_bytes_free();
		p->bytes_used = p->bytes_size;
		p->parent_offset = this->bytes_size - p->bytes_size;	// End of the device
		return Container::add_child (p, false);
	}

	return true;
}


bool
Partition::can_delete (QuestionPtr q)
{
	return_val_if_fail (q, false);

	if (get_count_real_children() > 1)
		return false;

	q->options.push_back ({
		Option::Type::checkbox,
		"delete.partition",
		std::string ("Delete ") + get_type(),
		get_device_name(),
		"1",
		get_smart(),
		false,
		false,
		-1, -1, -1, -1
	});

	ContainerPtr parent = get_parent();
	if (parent)
		return parent->can_delete(q);

	return false;
}
