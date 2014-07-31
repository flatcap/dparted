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

#include "volume.h"
#include "action.h"
#include "log.h"
#include "visitor.h"

Volume::Volume (void)
{
	log_ctor ("ctor Volume");
	const char* me = "Volume";

	sub_type (me);
}

Volume::Volume (const Volume& UNUSED(c)) :
	Volume()
{
	// No properties
}

Volume::Volume (Volume&& c)
{
	swap (c);
}

Volume::~Volume()
{
	log_dtor ("dtor Volume");
}

VolumePtr
Volume::create (void)
{
	VolumePtr p (new Volume());
	p->self = p;

	return p;
}


/**
 * operator= (copy)
 */
Volume&
Volume::operator= (const Volume& UNUSED(c))
{
	// No properties

	return *this;
}

/**
 * operator= (move)
 */
Volume&
Volume::operator= (Volume&& c)
{
	swap (c);
	return *this;
}


/**
 * swap (member)
 */
void
Volume::swap (Volume& UNUSED(c))
{
	// No properties
}

/**
 * swap (global)
 */
void
swap (Volume& lhs, Volume& rhs)
{
	lhs.swap (rhs);
}


Volume*
Volume::clone (void)
{
	LOG_TRACE;
	return new Volume (*this);
}

VolumePtr
Volume::copy (void)
{
	Volume *c = clone();

	VolumePtr cp (c);

	c->self = cp;

	return cp;
}


bool
Volume::accept (Visitor& v)
{
	VolumePtr p = std::dynamic_pointer_cast<Volume> (get_smart());
	if (!v.visit(p))
		return false;

	return visit_children(v);
}


std::vector<Action>
Volume::get_actions (void)
{
	LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.volume", true },
	};

	std::vector<Action> parent_actions = Whole::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
Volume::perform_action (Action action)
{
	if (action.name == "dummy.volume") {
		log_debug ("Volume perform: %s", action.name.c_str());
		return true;
	} else {
		return Whole::perform_action (action);
	}
}


