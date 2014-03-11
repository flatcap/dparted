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

#include <cstring>
#include <sstream>

#include "misc.h"
#include "action.h"
#include "log.h"
#include "log_trace.h"
#include "luks_table.h"
#include "main.h"
#include "partition.h"
#include "utils.h"
#include "visitor.h"

Misc::Misc (void)
{
	const char* me = "Misc";

	sub_type (me);
}

Misc::~Misc()
{
}

MiscPtr
Misc::create (void)
{
	MiscPtr p (new Misc());
	p->weak = p;

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
	// LOG_TRACE;
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
		std::cout << "Misc perform: " << action.name << std::endl;
		return true;
	} else {
		return Container::perform_action (action);
	}
}


static bool
is_empty (unsigned char* buffer, int bufsize)
{
	for (int i = 0; i < bufsize; i++) {
		if (buffer[i]) {
			return false;
		}
	}

	return true;
}

#if 0
static bool
is_random (unsigned char* buffer, int bufsize)
{
	double mean = 0;

	if (bufsize > 4096)
		bufsize = 4096;

	for (int i = 0; i < bufsize; i++) {
		mean += buffer[i];
	}

	mean /= bufsize;

	//log_debug ("data mean = %0.6f\n", mean);

	return ((mean > 125) && (mean < 130));
}

#endif

ContainerPtr
Misc::probe (ContainerPtr& top_level, ContainerPtr& parent)
{
	//LOG_TRACE;

	if (!parent)
		return nullptr;

	long		 bufsize = 1048576;	// 1 MiB
	unsigned char*	 buffer  = parent->get_buffer (0, bufsize);

	if (!buffer) {
		//log_error ("can't get buffer\n");
		return nullptr;
	}

	ContainerPtr c;

	c = LuksTable::probe (top_level, parent, buffer, bufsize);	//RAR
	if (c) {
		parent->add_child(c);

		c->bytes_size = parent->bytes_size;
		c->bytes_used = 0;
		c->parent_offset = 0;

		return c;
	}

	MiscPtr m;
	if (is_empty (buffer, bufsize)) {
		//log_error ("probe empty\n");
		m = Misc::create();
		m->name = "Zero";
		m->sub_type ("Zero");
	} else { //if (is_random (buffer, bufsize)) {
		//log_error ("probe random\n");
		m = Misc::create();
		m->name = "Random";
		m->sub_type ("Random");
	}

	if (m) {
		ContainerPtr c(m);
		parent->add_child(c);

		m->bytes_size = parent->bytes_size;
		m->bytes_used = 0;
		m->parent_offset = 0;
	}

	return m;
}


