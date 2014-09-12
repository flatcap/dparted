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

#include <algorithm>
#include <cstring>
#include <sstream>

#include "misc.h"
#include "action.h"
#include "log.h"
#include "partition.h"
#include "utils.h"
#include "visitor.h"

Misc::Misc (void)
{
	LOG_CTOR;
	const char* me = "Misc";

	sub_type (me);
}

Misc::Misc (const Misc& c) :
	Container(c)
{
	Misc();
	LOG_CTOR;
	// No properties
}

Misc::Misc (Misc&& c)
{
	LOG_CTOR;
	swap(c);
}

Misc::~Misc()
{
	LOG_DTOR;
}

MiscPtr
Misc::create (void)
{
	MiscPtr p (new Misc());
	p->self = p;

	return p;
}


Misc&
Misc::operator= (const Misc& UNUSED(c))
{
	// No properties

	return *this;
}

Misc&
Misc::operator= (Misc&& c)
{
	swap(c);
	return *this;
}


void
Misc::swap (Misc& UNUSED(c))
{
	// No properties
}

void
swap (Misc& lhs, Misc& rhs)
{
	lhs.swap (rhs);
}


Misc*
Misc::clone (void)
{
	LOG_TRACE;
	return new Misc (*this);
}


bool
Misc::accept (Visitor& v)
{
	MiscPtr m = std::dynamic_pointer_cast<Misc> (get_smart());
	if (!v.visit(m))
		return false;

	return visit_children(v);
}


std::vector<Action>
Misc::get_actions (void)
{
	LOG_TRACE;

	ContainerPtr me = get_smart();
	std::vector<Action> actions = {
		{ "dummy.misc", "Dummy/Misc", me, true },
	};

	std::vector<Action> base_actions = Container::get_actions();

	actions.insert (std::end (actions), std::begin (base_actions), std::end (base_actions));

	return actions;
}

bool
Misc::perform_action (Action action)
{
	if (action.name == "dummy.misc") {
		log_debug ("Misc perform: %s", SP(action.name));
		return true;
	} else {
		return Container::perform_action (action);
	}
}


static bool
is_empty (std::uint8_t* buffer, int bufsize)
{
	std::uint64_t* lptr = (std::uint64_t*) buffer;

	bufsize /= sizeof (std::uint64_t);

	for (int i = 0; i < bufsize; ++i) {
		if (lptr[i]) {
			return false;
		}
	}

	return true;
}

#if 0
static bool
is_random (std::uint8_t* buffer, int bufsize)
{
	double mean = 0;

	for (int i = 0; i < bufsize; ++i) {
		mean += buffer[i];
	}

	mean /= bufsize;

	log_debug ("data mean = %0.6f", mean);

	return ((mean > 125) && (mean < 130));
}
#endif

bool
Misc::probe (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize)
{
	return_val_if_fail (parent,  false);
	return_val_if_fail (buffer,  false);
	return_val_if_fail (bufsize, false);
	LOG_TRACE;

	// Let's not go crazy and examine the whole device:
	bufsize = std::min (bufsize, (std::uint64_t) 102400);		// 100KiB

	PartitionPtr p;
	if (is_empty (buffer, bufsize)) {
		log_error ("probe empty");
		p = Partition::create();
		p->sub_type ("Space");
		p->sub_type ("Unallocated");
		// p->sub_type ("Zero");
	} else { // if (is_random (buffer, bufsize)) {
		log_error ("probe random");
		p = Partition::create();
		p->sub_type ("Space");
		p->sub_type ("Unallocated");
		// p->sub_type ("Random");
	}

	if (p) {
		ContainerPtr new_parent = Container::start_transaction (parent, "Misc: probe");
		if (!new_parent) {
			log_error ("misc probe failed");
			return false;
		}

		p->bytes_size = new_parent->bytes_size;
		p->bytes_used = p->bytes_size;
		p->parent_offset = 0;

		if (new_parent->add_child (p, false)) {
			Container::commit_transaction();
			return true;
		} else {
			Container::cancel_transaction();
			return false;
		}

	}

	return false;
}


