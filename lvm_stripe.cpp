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
#include "log_trace.h"
#include "visitor.h"

/**
 * LvmStripe
 */
LvmStripe::LvmStripe (void)
{
	const char* me = "LvmStripe";

	sub_type (me);
}

/**
 * create
 */
LvmStripePtr
LvmStripe::create (void)
{
	LvmStripePtr l (new LvmStripe());
	l->weak = l;

	return l;
}


/**
 * accept
 */
bool
LvmStripe::accept (Visitor& v)
{
	LvmStripePtr l = std::dynamic_pointer_cast<LvmStripe> (get_smart());
	if (!v.visit(l))
		return false;
	return visit_children(v);
}


