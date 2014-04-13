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
#include "log_trace.h"
#include "main.h"
#include "partition.h"
#include "utils.h"
#include "visitor.h"

Misc::Misc (void)
{
	log_ctor ("ctor Misc");
	const char* me = "Misc";

	sub_type (me);
}

Misc::~Misc()
{
	log_dtor ("dtor Misc");
}

MiscPtr
Misc::create (void)
{
	MiscPtr p (new Misc());
	p->self = p;

	return p;
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
	std::vector<Action> actions = {
		{ "dummy.misc", true },
	};

	std::vector<Action> parent_actions = Container::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
Misc::perform_action (Action action)
{
	if (action.name == "dummy.misc") {
		log_debug ("Misc perform: %s", action.name.c_str());
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

	MiscPtr m;
	if (is_empty (buffer, bufsize)) {
		log_error ("probe empty");
		m = Misc::create();
		m->sub_type ("Zero");
	} else { //if (is_random (buffer, bufsize)) {
		log_error ("probe random");
		m = Misc::create();
		m->sub_type ("Random");
	}

	if (m) {
		parent->add_child(m);

		m->bytes_size = parent->bytes_size;
		m->bytes_used = 0;
		m->parent_offset = 0;
		return true;
	}

	return false;
}


