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
#include "container_listener.h"

enum class EventType {
	t_add,		// A container was added
	t_delete,	// ... deleted
	t_edit		// ... changed
};

typedef std::tuple<std::chrono::steady_clock::time_point,EventType,ContainerPtr,ContainerPtr,std::string> Event;	// Date, event type, old, new, description

class Timeline;

typedef std::shared_ptr<Timeline> TimelinePtr;

class Timeline : public IContainerListener
{
public:
	static TimelinePtr create (ContainerPtr& cont);
	virtual ~Timeline();

protected:
	Timeline (void);

	virtual void container_added   (const ContainerPtr& cont, const ContainerPtr& parent, const char* description);
	virtual void container_busy    (const ContainerPtr& cont, int busy);
	virtual void container_changed (const ContainerPtr& cont);
	virtual void container_deleted (const ContainerPtr& cont);
	virtual void container_resync  (const ContainerPtr& cont);

	std::weak_ptr<Timeline> self;

	std::vector<Event> event_list;
};

#endif // _TIMELINE_H_

