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
	LOG_CTOR;
	const char* me = "Volume";

	sub_type (me);
}

Volume::Volume (const Volume& c) :
	Whole(c)
{
	Volume();
	LOG_CTOR;
	// No properties
}

Volume::Volume (Volume&& c)
{
	LOG_CTOR;
	swap(c);
}

Volume::~Volume()
{
	LOG_DTOR;
}

VolumePtr
Volume::create (void)
{
	VolumePtr p (new Volume());
	p->self = p;

	return p;
}


Volume&
Volume::operator= (const Volume& UNUSED(c))
{
	// No properties

	return *this;
}

Volume&
Volume::operator= (Volume&& c)
{
	swap(c);
	return *this;
}


void
Volume::swap (Volume& UNUSED(c))
{
	// No properties
}

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

	std::vector<Action> base_actions = Whole::get_actions();

	actions.insert (std::end (actions), std::begin (base_actions), std::end (base_actions));

	return actions;
}

bool
Volume::perform_action (Action action)
{
	if (action.name == "dummy.volume") {
		log_debug ("Volume perform: %s", SP(action.name));
		return true;
	} else {
		return Whole::perform_action (action);
	}
}


