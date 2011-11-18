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


#ifndef _LVM_H_
#define _LVM_H_

#include "container.h"

#include <glibmm/ustring.h>

class LVM : public Container
{
public:
	LVM (void);
	virtual ~LVM();

	virtual void Dump (void);

protected:

private:
	Glib::ustring	vg_name;
	int		pv_count;
	int		lv_count;
	long		vg_attr;
	long long	vg_size;
	long long	vg_free;
	Glib::ustring	vg_uuid;
	long long	vg_extent_size;
	long long	vg_extent_count;
	long long	vg_free_count;
	long		vg_seqno;
};

#endif /* _LVM_H_ */

