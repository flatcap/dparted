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

#include "whole.h"
#include "action.h"
#include "log.h"
#include "log_trace.h"
#include "visitor.h"

Whole::Whole (void)
{
	const char* me = "Whole";

	sub_type (me);
}

Whole::~Whole()
{
}

WholePtr
Whole::create (void)
{
	WholePtr p (new Whole());
	p->self = p;

	return p;
}


bool
Whole::accept (Visitor& v)
{
	WholePtr w = std::dynamic_pointer_cast<Whole> (get_smart());
	if (!v.visit(w))
		return false;

	return visit_children(v);
}


std::vector<Action>
Whole::get_actions (void)
{
	LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.whole", true },
	};

	std::vector<Action> parent_actions = Container::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
Whole::perform_action (Action action)
{
	if (action.name == "dummy.whole") {
		log_debug ("Whole perform: %s\n", action.name.c_str());
		return true;
	} else {
		return Container::perform_action (action);
	}
}


void
Whole::add_segment (ContainerPtr seg)
{
	segments.insert (seg);

	seg->whole = get_smart();
}


