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

#ifndef _TIMELINE_H_
#define _TIMELINE_H_

#include <memory>
#include <tuple>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>

#include "container.h"

enum class EventType {
	t_add,		// A container was added
	t_delete,	// ... deleted
	t_edit		// ... changed
};

typedef std::tuple<std::chrono::steady_clock::time_point,EventType,ContainerPtr,ContainerPtr,std::string> Event;	// Date, event type, old, new, description

typedef std::shared_ptr<class Timeline> TimelinePtr;

class Timeline
{
public:
	static TimelinePtr create (ContainerPtr& cont);
	virtual ~Timeline();

	bool adjust (int amount);

protected:
	Timeline (void);

	std::weak_ptr<Timeline> self;

	std::vector<Event> event_list;
	// event_cursor points to the end of the event_list unless we're
	// re-wound time.  In that case it points to the first redo item.
	std::vector<Event>::iterator event_cursor;
};

#endif // _TIMELINE_H_

