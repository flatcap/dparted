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
	LOG_CTOR;
	const char* me = "LvmStripe";

	sub_type (me);
}

LvmStripe::LvmStripe (const LvmStripe& c) :
	LvmVolume(c)
{
	LvmStripe();
	LOG_CTOR;
	// No properties
}

LvmStripe::LvmStripe (LvmStripe&& c)
{
	LOG_CTOR;
	swap(c);
}

LvmStripe::~LvmStripe()
{
	LOG_DTOR;
}

LvmStripePtr
LvmStripe::create (void)
{
	LvmStripePtr p (new LvmStripe());
	p->self = p;

	return p;
}


LvmStripe&
LvmStripe::operator= (const LvmStripe& UNUSED(c))
{
	// No properties

	return *this;
}

LvmStripe&
LvmStripe::operator= (LvmStripe&& c)
{
	swap(c);
	return *this;
}


void
LvmStripe::swap (LvmStripe& UNUSED(c))
{
	// No properties
}

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

	ContainerPtr me = get_smart();
	std::vector<Action> actions = {
		{ "dummy.lvm_stripe", "Dummy/Lvm Stripe", me, true },
	};

	std::vector<Action> base_actions = LvmVolume::get_actions();

	actions.insert (std::end (actions), std::begin (base_actions), std::end (base_actions));

	return actions;
}

bool
LvmStripe::perform_action (Action action)
{
	if (action.name == "dummy.lvm_stripe") {
		log_debug ("LvmStripe perform: %s", SP(action.name));
		return true;
	} else {
		return LvmVolume::perform_action (action);
	}
}

