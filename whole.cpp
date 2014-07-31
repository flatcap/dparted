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
#include "visitor.h"

Whole::Whole (void)
{
	log_ctor ("ctor Whole");
	const char* me = "Whole";

	sub_type (me);
}

Whole::Whole (const Whole& c) :
	Whole()
{
	segments = c.segments;
}

Whole::Whole (Whole&& c)
{
	swap (c);
}

Whole::~Whole()
{
	log_dtor ("dtor Whole");
}

WholePtr
Whole::create (void)
{
	WholePtr p (new Whole());
	p->self = p;

	return p;
}


/**
 * operator= (copy)
 */
Whole&
Whole::operator= (const Whole& c)
{
	segments = c.segments;

	return *this;
}

/**
 * operator= (move)
 */
Whole&
Whole::operator= (Whole&& c)
{
	swap (c);
	return *this;
}


/**
 * swap (member)
 */
void
Whole::swap (Whole& c)
{
	std::swap (segments, c.segments);
}

/**
 * swap (global)
 */
void
swap (Whole& lhs, Whole& rhs)
{
	lhs.swap (rhs);
}


Whole*
Whole::clone (void)
{
	LOG_TRACE;
	return new Whole (*this);
}

WholePtr
Whole::copy (void)
{
	Whole *c = clone();

	WholePtr cp (c);

	c->self = cp;

	return cp;
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
		log_debug ("Whole perform: %s", action.name.c_str());
		return true;
	} else {
		return Container::perform_action (action);
	}
}


void
Whole::add_segment (ContainerPtr seg)
{
	segments.insert (seg);

	// seg->whole = get_smart();	//XXX this belongs here, but whole can't be protected
}


