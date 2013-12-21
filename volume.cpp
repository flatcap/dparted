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

#include "log.h"
#include "volume.h"
#include "log_trace.h"
#include "visitor.h"

/**
 * create
 */
VolumePtr
Volume::create (void)
{
	VolumePtr v (new Volume());
	v->declare ("volume");
	v->weak = v;

	return v;
}

/**
 * accept
 */
bool
Volume::accept (Visitor& v)
{
	VolumePtr p = std::dynamic_pointer_cast<Volume> (get_smart());
	if (!v.visit (p))
		return false;
	return visit_children (v);
}

