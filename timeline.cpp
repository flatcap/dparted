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
#include "container.h"

Timeline::Timeline (void)
{
	LOG_CTOR;
}

Timeline::~Timeline()
{
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	for (auto& e : event_list) {
		std::chrono::steady_clock::time_point then = std::get<0>(e);
		int ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - then).count();
		log_debug ("%3dms ago (%d): %p/%p : %s",
			ms,
			std::get<1>(e),
			std::get<2>(e).get(),
			std::get<3>(e).get(),
			std::get<4>(e).c_str());
	}
	LOG_DTOR;
}

TimelinePtr
Timeline::create (ContainerPtr& cont)
{
	TimelinePtr p (new Timeline());
	p->self = p;
	cont->add_listener(p);

	return p;
}


void
Timeline::container_added (const ContainerPtr& parent, const ContainerPtr& cont, const char* description)
{
	LOG_TRACE;

	if (event_cursor != std::end (event_list)) {
		log_error ("TIMELINE CURRENTLY REWOUND");
		return;
	}

	std::string desc;
	if (description) {
		desc = description;
	} else {
		desc = "Unknown event";
		desc = "";
	}

	event_list.push_back (std::make_tuple (std::chrono::steady_clock::now(), EventType::t_add, parent, cont, desc));
	event_cursor = std::end (event_list);
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


bool
Timeline::adjust (int amount)
{
	return_val_if_fail (amount, false);
	// log_code ("adjust timeline %+d", amount);

	if (amount < 0) {
		if (event_cursor == std::begin (event_list)) {
			log_code ("already at the beginning");
			return false;
		}

		event_cursor--;

		ContainerPtr cold = std::get<2>(*event_cursor);
		ContainerPtr cnew = std::get<3>(*event_cursor);

		log_info ("Undo event: %s", std::get<4>(*event_cursor).c_str());
		exchange (cold, cnew);
	} else {
		if (event_cursor == std::end (event_list)) {
			log_info ("already at the end");
			return false;
		}

		ContainerPtr cold = std::get<2>(*event_cursor);
		ContainerPtr cnew = std::get<3>(*event_cursor);

		log_info ("Redo event: %s", std::get<4>(*event_cursor).c_str());
		exchange (cold, cnew);

		event_cursor++;
	}
	return false;
}

