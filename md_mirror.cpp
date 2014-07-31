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

#include "md_mirror.h"
#include "action.h"
#include "visitor.h"

MdMirror::MdMirror (void)
{
	log_ctor ("ctor MdMirror");
	const char* me = "MdMirror";

	sub_type (me);
}

MdMirror::MdMirror (const MdMirror& UNUSED(c)) :
	MdMirror()
{
	// No properties
}

MdMirror::MdMirror (MdMirror&& c)
{
	swap (c);
}

MdMirror::~MdMirror()
{
	log_dtor ("dtor MdMirror");
}

MdMirrorPtr
MdMirror::create (void)
{
	MdMirrorPtr p (new MdMirror());
	p->self = p;

	return p;
}


/**
 * operator= (copy)
 */
MdMirror&
MdMirror::operator= (const MdMirror& UNUSED(c))
{
	// No properties

	return *this;
}

/**
 * operator= (move)
 */
MdMirror&
MdMirror::operator= (MdMirror&& c)
{
	swap (c);
	return *this;
}


/**
 * swap (member)
 */
void
MdMirror::swap (MdMirror& UNUSED(c))
{
	// No properties
}

/**
 * swap (global)
 */
void
swap (MdMirror& lhs, MdMirror& rhs)
{
	lhs.swap (rhs);
}


MdMirror*
MdMirror::clone (void)
{
	LOG_TRACE;
	return new MdMirror (*this);
}


bool
MdMirror::accept (Visitor& v)
{
	MdMirrorPtr b = std::dynamic_pointer_cast<MdMirror> (get_smart());
	if (!v.visit(b))
		return false;

	return visit_children(v);
}


std::vector<Action>
MdMirror::get_actions (void)
{
	LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.md_mirror", true },
	};

	std::vector<Action> parent_actions = MdVolume::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
MdMirror::perform_action (Action action)
{
	if (action.name == "dummy.md_mirror") {
		log_debug ("MdMirror perform: %s", action.name.c_str());
		return true;
	} else {
		return MdVolume::perform_action (action);
	}
}


