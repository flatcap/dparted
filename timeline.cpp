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
#include "log.h"
#include "utils.h"

Timeline::Timeline (void)
{
}

Timeline::~Timeline()
{
	for (auto& e : event_list) {
		log_code ("%s (%d): %p/%p : %s",
			std::get<0>(e).c_str(),
			(int) std::get<1>(e),
			std::get<2>(e).get(),
			std::get<3>(e).get(),
			std::get<4>(e).c_str());
	}
}

TimelinePtr
Timeline::create (ContainerPtr& cont)
{
	TimelinePtr p (new Timeline());
	p->self = p;
	cont->add_listener (p);

	return p;
}


void
Timeline::container_added (const ContainerPtr& cont, const ContainerPtr& UNUSED(parent), const char* description)
{
	LOG_TRACE;
	event_list.push_back (std::make_tuple ("2014-07-26 23:43:08", EventType::t_add, nullptr, cont, description));
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


