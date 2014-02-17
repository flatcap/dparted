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

#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <string>
#include <memory>

#include "container.h"
#include "action.h"

class Device;
class Visitor;

typedef std::shared_ptr<Device> DevicePtr;

/**
 * class Device
 */
class Device : public virtual Container
{
public:
	virtual ~Device() = default;
	static DevicePtr create (void);
	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

public:
	int kernel_major = 0;	//XXX rename device major
	int kernel_minor = 0;

protected:
	Device (void);

private:

};


#endif // _DEVICE_H_

