/* Copyright (c) 2013 Richard Russon (FlatCap)
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Library General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place - Suite 330, Boston, MA 02111-1307, USA.
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

	declare (me);
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
	if (!v.visit (l))
		return false;
	return visit_children (v);
}


