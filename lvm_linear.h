/* Copyright (c) 2014 Richard Russon (FlatCap)
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

#ifndef _LVM_LINEAR_H_
#define _LVM_LINEAR_H_

#include <memory>

#include "lvm_volume.h"

class LvmLinear;
class Visitor;

typedef std::shared_ptr<LvmLinear> LvmLinearPtr;

/**
 * class LvmLinear
 */
class LvmLinear : public LvmVolume
{
public:
	virtual ~LvmLinear() = default;
	static LvmLinearPtr create (void);
	virtual bool accept (Visitor& v);

public:
	//properties

protected:
	LvmLinear (void);

private:

};


#endif // _LVM_LINEAR_H_

