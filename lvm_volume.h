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

#ifndef _LVM_VOLUME_H_
#define _LVM_VOLUME_H_

#include "volume.h"
#include "pointers.h"

class Visitor;

/**
 * class LvmVolume
 */
class LvmVolume : public Volume
{
public:
	virtual ~LvmVolume() = default;
	static LvmVolumePtr create (void);
	virtual bool accept (Visitor& v);

	virtual void add_child (ContainerPtr& child);

	virtual ContainerPtr find (const std::string& uuid);

public:
	std::string	lv_attr;
	long		kernel_major = -1;
	long		kernel_minor = -1;

	int		seg_count    = 0;
	int		stripes      = 0;
	int		stripesize   = 0;
	int		stripe_size  = 0;
	int		seg_start_pe = 0;

	std::string	mirror_log;

	std::vector<ContainerPtr> metadata;
	std::vector<ContainerPtr> subvols;

	ContainerPtr sibling;

protected:
	LvmVolume (void);

private:

};


#endif // _LVM_VOLUME_H_

