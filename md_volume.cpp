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

#include "md_volume.h"
#include "action.h"
#include "visitor.h"

MdVolume::MdVolume (void)
{
	LOG_CTOR;
	const char* me = "MdVolume";

	sub_type (me);
}

MdVolume::MdVolume (const MdVolume& c) :
	Volume(c)
{
	MdVolume();
	LOG_CTOR;
	// No properties
}

MdVolume::MdVolume (MdVolume&& c)
{
	LOG_CTOR;
	swap (c);
}

MdVolume::~MdVolume()
{
	LOG_DTOR;
}

MdVolumePtr
MdVolume::create (void)
{
	MdVolumePtr p (new MdVolume());
	p->self = p;

	return p;
}


/**
 * operator= (copy)
 */
MdVolume&
MdVolume::operator= (const MdVolume& UNUSED(c))
{
	// No properties

	return *this;
}

/**
 * operator= (move)
 */
MdVolume&
MdVolume::operator= (MdVolume&& c)
{
	swap (c);
	return *this;
}


/**
 * swap (member)
 */
void
MdVolume::swap (MdVolume& UNUSED(c))
{
	// No properties
}

/**
 * swap (global)
 */
void
swap (MdVolume& lhs, MdVolume& rhs)
{
	lhs.swap (rhs);
}


MdVolume*
MdVolume::clone (void)
{
	LOG_TRACE;
	return new MdVolume (*this);
}


bool
MdVolume::accept (Visitor& v)
{
	MdVolumePtr b = std::dynamic_pointer_cast<MdVolume> (get_smart());
	if (!v.visit(b))
		return false;

	return visit_children(v);
}


std::vector<Action>
MdVolume::get_actions (void)
{
	LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.md_volume", true },
	};

	std::vector<Action> parent_actions = Volume::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
MdVolume::perform_action (Action action)
{
	if (action.name == "dummy.md_volume") {
		log_debug ("MdVolume perform: %s", action.name.c_str());
		return true;
	} else {
		return Volume::perform_action (action);
	}
}


