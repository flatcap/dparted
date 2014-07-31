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

#include "lvm_stripe.h"
#include "action.h"
#include "log.h"
#include "visitor.h"

LvmStripe::LvmStripe (void)
{
	log_ctor ("ctor LvmStripe");
	const char* me = "LvmStripe";

	sub_type (me);
}

LvmStripe::LvmStripe (const LvmStripe& UNUSED(c)) :
	LvmStripe()
{
	// No properties
}

LvmStripe::LvmStripe (LvmStripe&& c)
{
	swap (c);
}

LvmStripe::~LvmStripe()
{
	log_dtor ("dtor LvmStripe");
}

LvmStripePtr
LvmStripe::create (void)
{
	LvmStripePtr p (new LvmStripe());
	p->self = p;

	return p;
}


/**
 * operator= (copy)
 */
LvmStripe&
LvmStripe::operator= (const LvmStripe& UNUSED(c))
{
	// No properties

	return *this;
}

/**
 * operator= (move)
 */
LvmStripe&
LvmStripe::operator= (LvmStripe&& c)
{
	swap (c);
	return *this;
}


/**
 * swap (member)
 */
void
LvmStripe::swap (LvmStripe& UNUSED(c))
{
	// No properties
}

/**
 * swap (global)
 */
void
swap (LvmStripe& lhs, LvmStripe& rhs)
{
	lhs.swap (rhs);
}


LvmStripe*
LvmStripe::clone (void)
{
	LOG_TRACE;
	return new LvmStripe (*this);
}


bool
LvmStripe::accept (Visitor& v)
{
	LvmStripePtr l = std::dynamic_pointer_cast<LvmStripe> (get_smart());
	if (!v.visit(l))
		return false;

	return visit_children(v);
}


std::vector<Action>
LvmStripe::get_actions (void)
{
	LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.lvm_stripe", true },
	};

	std::vector<Action> parent_actions = LvmVolume::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
LvmStripe::perform_action (Action action)
{
	if (action.name == "dummy.lvm_stripe") {
		log_debug ("LvmStripe perform: %s", action.name.c_str());
		return true;
	} else {
		return LvmVolume::perform_action (action);
	}
}

