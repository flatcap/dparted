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

#ifndef _LVM_MIRROR_H_
#define _LVM_MIRROR_H_

#include "lvm_volume.h"
#include "pointers.h"

class Visitor;

/**
 * class LvmMirror
 */
class LvmMirror : public LvmVolume
{
public:
	static LvmMirrorPtr create (void);
	virtual bool accept (Visitor& v);

	virtual void add_child    (ContainerPtr child);
	virtual void delete_child (ContainerPtr child);

#if 0
	std::vector<ContainerPtr> mirrors;
	ContainerPtr log;
#endif
protected:
private:

};


#endif // _LVM_MIRROR_H_

