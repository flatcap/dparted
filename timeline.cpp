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
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	for (auto& e : event_list) {
		std::chrono::steady_clock::time_point then = std::get<0>(e);
		int ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - then).count();
		log_code ("%3dms ago (%d): %p/%p : %s",
			ms,
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
	std::string desc;
	if (description) {
		desc = description;
	} else {
		desc = "Unknown event";
		desc = "";
	}

	event_list.push_back (std::make_tuple (std::chrono::steady_clock::now(), EventType::t_add, nullptr, cont, desc));
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


