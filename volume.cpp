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

#include "log.h"
#include "volume.h"
#include "log_trace.h"
#include "visitor.h"

/**
 * Volume
 */
Volume::Volume (void)
{
	const char* me = "Volume";

	sub_type (me);
}

/**
 * create
 */
VolumePtr
Volume::create (void)
{
	VolumePtr v (new Volume());
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
	if (!v.visit(p))
		return false;
	return visit_children(v);
}

