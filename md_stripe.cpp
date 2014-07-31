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
	log_ctor ("ctor MdStripe");
	const char* me = "MdStripe";

	sub_type (me);
}

MdStripe::MdStripe (const MdStripe& UNUSED(c)) :
	MdStripe()
{
	// No properties
}

MdStripe::MdStripe (MdStripe&& c)
{
	swap (c);
}

MdStripe::~MdStripe()
{
	log_dtor ("dtor MdStripe");
}

MdStripePtr
MdStripe::create (void)
{
	MdStripePtr p (new MdStripe());
	p->self = p;

	return p;
}


/**
 * operator= (copy)
 */
MdStripe&
MdStripe::operator= (const MdStripe& UNUSED(c))
{
	// No properties

	return *this;
}

/**
 * operator= (move)
 */
MdStripe&
MdStripe::operator= (MdStripe&& c)
{
	swap (c);
	return *this;
}


/**
 * swap (member)
 */
void
MdStripe::swap (MdStripe& UNUSED(c))
{
	// No properties
}

/**
 * swap (global)
 */
void
swap (MdStripe& lhs, MdStripe& rhs)
{
	lhs.swap (rhs);
}


MdStripe*
MdStripe::clone (void)
{
	return new MdStripe (*this);
}

MdStripePtr
MdStripe::copy (void)
{
	MdStripe *c = clone();

	MdStripePtr cp (c);

	c->self = cp;

	return cp;
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

	std::vector<Action> parent_actions = MdVolume::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
MdStripe::perform_action (Action action)
{
	if (action.name == "dummy.md_stripe") {
		log_debug ("MdStripe perform: %s", action.name.c_str());
		return true;
	} else {
		return MdVolume::perform_action (action);
	}
}


