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

#ifndef _LVM_GROUP_H_
#define _LVM_GROUP_H_

#include <memory>

#include "whole.h"

class LvmGroup;
class LvmTable;
class Visitor;

typedef std::shared_ptr<LvmGroup> LvmGroupPtr;

/**
 * class LvmGroup
 */
class LvmGroup : public Whole
{
public:
	virtual ~LvmGroup() = default;
	static LvmGroupPtr create (void);
	virtual bool accept (Visitor& v);

	//virtual void add_segment (ContainerPtr& seg);

	static void discover (ContainerPtr& top_level);

public:
	long		pv_count = 0;	//XXX put this in seg_count in Whole
	long		lv_count = 0;	//XXX this matches children.size()
	long		vg_seqno = 0;
	std::string	vg_attr;

	// vector of components, e.g. /dev/loop0, /dev/loop1, ...
	//std::vector<std::string> components;	//XXX do we need this, shouldn't we just use a the segments vector?

protected:
	LvmGroup (void);

	friend void lvm_pvs (ContainerPtr& pieces, std::multimap<std::string,std::string>& deps);
	friend void lvm_vgs (ContainerPtr& pieces, std::multimap<std::string,std::string>& deps);
	friend void lvm_lvs (ContainerPtr& pieces, std::multimap<std::string,std::string>& deps);

private:

};


#endif // _LVM_GROUP_H_

