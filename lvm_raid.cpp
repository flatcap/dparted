/* Copyright (c) 2014 Richard Russon (FlatCap)
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

#include "lvm_raid.h"
#include "log.h"
#include "log_trace.h"
#include "visitor.h"

/**
 * LvmRaid
 */
LvmRaid::LvmRaid (void)
{
	const char* me = "LvmRaid";

	sub_type (me);
}

/**
 * create
 */
LvmRaidPtr
LvmRaid::create (void)
{
	LvmRaidPtr l (new LvmRaid());
	l->weak = l;

	return l;
}


/**
 * accept
 */
bool
LvmRaid::accept (Visitor& v)
{
	LvmRaidPtr l = std::dynamic_pointer_cast<LvmRaid> (get_smart());
	if (!v.visit(l))
		return false;
	return visit_children(v);
}


