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

#ifndef _PARTITION_H_
#define _PARTITION_H_

#include <memory>
#include <string>
#include <vector>

#include "container.h"
#include "volume.h"

class Partition;

typedef std::shared_ptr<Partition> PartitionPtr;

/**
 * class Partition - A subdivision of a Block
 */
class Partition : public Container
{
public:
	static PartitionPtr create (void);
	virtual ~Partition();
	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);
	virtual void add_child (ContainerPtr& child);

public:
	//properties
	int ptype = 0;

	VolumePtr volume;	//XXX not declared

	//XXX flag for protective partition
	//XXX partition number - nth in set?
	//XXX partition type - 0x82 Linux Swap

protected:
	Partition (void);

private:

};

#endif // _PARTITION_H_

