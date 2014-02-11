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

#ifndef _LVM_METADATA_H_
#define _LVM_METADATA_H_

#include <memory>

#include "lvm_linear.h"

class LvmMetadata;
class Visitor;

typedef std::shared_ptr<LvmMetadata> LvmMetadataPtr;

/**
 * class LvmMetadata
 */
class LvmMetadata : public LvmLinear
{
public:
	virtual ~LvmMetadata() = default;
	static LvmMetadataPtr create (void);
	virtual bool accept (Visitor& v);

	virtual void add_child (ContainerPtr& child);

public:
	//properties

protected:
	LvmMetadata (void);

private:

};


#endif // _LVM_METADATA_H_

