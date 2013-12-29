/* Copyright (c) 2013 Richard Russon (FlatCap)
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Library General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _PARTITION_H_
#define _PARTITION_H_

#include <string>
#include <memory>

#include "container.h"
#include "volume.h"

class Partition;
class Visitor;
class Volume;

typedef std::shared_ptr<Partition> PartitionPtr;

/**
 * class Partition
 */
class Partition : public Container
{
public:
	virtual ~Partition() = default;
	static PartitionPtr create (void);
	virtual bool accept (Visitor& v);

public:
	int ptype = 0;

	VolumePtr volume;

	//XXX flag for protective partition
	//XXX partition number - nth in set?
	//XXX partition type - 0x82 Linux Swap

protected:
	Partition (void);

private:

};


#endif // _PARTITION_H_

