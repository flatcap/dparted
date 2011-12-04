/* Copyright (c) 2011 Richard Russon (FlatCap)
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

	std::string	vg_name;
	int		pv_count;
	int		lv_count;
	std::string	vg_attr;
	long long	vg_size;
	long long	vg_free;
	std::string	vg_uuid;
	long long	vg_extent_size;
	long long	vg_extent_count;
	long long	vg_free_count;
	long		vg_seqno;

	//std::vector<std::string> devices;

protected:

private:

};

#endif /* _VOLUMEGROUP_H_ */

