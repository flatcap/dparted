/* Copyright (c) 2012 Richard Russon (FlatCap)
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


#ifndef _VOLUMEGROUP_H_
#define _VOLUMEGROUP_H_

#include <string>

#include "whole.h"

class VolumeGroup : public Whole
{
public:
	VolumeGroup (void);
	virtual ~VolumeGroup();

	virtual void dump (int indent = 0);
	virtual void dump_csv (void);
	virtual std::string dump_dot (void);

	static void find_devices (Container &list);

	long		pv_count;	//XXX put this in seg_count in Whole
	long		lv_count;	//XXX this matches children.size()
	std::string	vg_attr;
	long long	vg_extent_count;
	long long	vg_free_count;
	long		vg_seqno;

	// vector of components, e.g. /dev/loop0, /dev/loop1, ...
	//std::vector<std::string> components;	//XXX do we need this, shouldn't we just use a the segments vector?

protected:

private:

};

#endif /* _VOLUMEGROUP_H_ */

