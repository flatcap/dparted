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

#include "timeline.h"
#include "utils.h"

Timeline::Timeline (void)
{
}

Timeline::~Timeline()
{
}

TimelinePtr
Timeline::create (void)
{
	TimelinePtr p (new Timeline());
	p->self = p;

	return p;
}


void
Timeline::container_added (const ContainerPtr& UNUSED(cont), const ContainerPtr& UNUSED(parent))
{
	LOG_TRACE;
}

void
Timeline::container_busy (const ContainerPtr& UNUSED(cont), int UNUSED(busy))
{
	LOG_TRACE;
}

void
Timeline::container_changed (const ContainerPtr& UNUSED(cont))
{
	LOG_TRACE;
}

void
Timeline::container_deleted (const ContainerPtr& UNUSED(cont))
{
	LOG_TRACE;
}

void
Timeline::container_resync (const ContainerPtr& UNUSED(cont))
{
	LOG_TRACE;
}


