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

#ifndef _CONTAINER_LISTENER_H_
#define _CONTAINER_LISTENER_H_

#include <memory>

typedef std::shared_ptr<class Container> ContainerPtr;
typedef std::shared_ptr<class IContainerListener> ContainerListenerPtr;
typedef std::weak_ptr<class IContainerListener> ContainerListenerWeak;

class IContainerListener
{
public:
	virtual ~IContainerListener() = default;

	virtual void container_added   (const ContainerPtr& parent, const ContainerPtr& child) = 0;
	virtual void container_changed (const ContainerPtr& parent, const ContainerPtr& before, const ContainerPtr& after) = 0;
#if 0
	virtual void container_added   (const ContainerPtr& parent, const ContainerPtr& cont, const char* description) = 0;
	virtual void container_busy    (const ContainerPtr& cont, int busy) = 0;
	virtual void container_changed (const ContainerPtr& cont) = 0;
	virtual void container_deleted (const ContainerPtr& cont) = 0;
	virtual void container_resync  (const ContainerPtr& cont) = 0;
#endif
};

#endif // _CONTAINER_LISTENER_H_

