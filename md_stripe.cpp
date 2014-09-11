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

#include "md_stripe.h"
#include "action.h"
#include "visitor.h"

MdStripe::MdStripe (void)
{
	LOG_CTOR;
	const char* me = "MdStripe";

	sub_type (me);
}

MdStripe::MdStripe (const MdStripe& c) :
	MdVolume(c)
{
	MdStripe();
	LOG_CTOR;
	// No properties
}

MdStripe::MdStripe (MdStripe&& c)
{
	LOG_CTOR;
	swap(c);
}

MdStripe::~MdStripe()
{
	LOG_DTOR;
}

MdStripePtr
MdStripe::create (void)
{
	MdStripePtr p (new MdStripe());
	p->self = p;

	return p;
}


MdStripe&
MdStripe::operator= (const MdStripe& UNUSED(c))
{
	// No properties

	return *this;
}

MdStripe&
MdStripe::operator= (MdStripe&& c)
{
	swap(c);
	return *this;
}


void
MdStripe::swap (MdStripe& UNUSED(c))
{
	// No properties
}

void
swap (MdStripe& lhs, MdStripe& rhs)
{
	lhs.swap (rhs);
}


MdStripe*
MdStripe::clone (void)
{
	LOG_TRACE;
	return new MdStripe (*this);
}


bool
MdStripe::accept (Visitor& v)
{
	MdStripePtr b = std::dynamic_pointer_cast<MdStripe> (get_smart());
	if (!v.visit(b))
		return false;

	return visit_children(v);
}


std::vector<Action>
MdStripe::get_actions (void)
{
	LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.md_stripe", true },
	};

	std::vector<Action> base_actions = MdVolume::get_actions();

	actions.insert (std::end (actions), std::begin (base_actions), std::end (base_actions));

	return actions;
}

bool
MdStripe::perform_action (Action action)
{
	if (action.name == "dummy.md_stripe") {
		log_debug ("MdStripe perform: %s", SP(action.name));
		return true;
	} else {
		return MdVolume::perform_action (action);
	}
}


