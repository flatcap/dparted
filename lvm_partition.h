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

#ifndef _LVM_PARTITION_H_
#define _LVM_PARTITION_H_

#include "partition.h"
#include "pointers.h"

/**
 * class LvmPartition
 */
class LvmPartition : public Partition
{
public:
	static LvmPartitionPtr create (void);
	virtual ~LvmPartition() = default;

#if 0
	long		dev_size;	// 5368709120
	long		pe_start;	// 1048576
	std::string	pv_attr;	// a--
	long		pv_count;	// 8
	long		pv_free;	// 4294967296
	std::string	pv_name;	// /dev/loop2
	long		pv_pe_alloc;	// 255
	long		pv_pe_count;	// 1279
	long		pv_size;	// 5364514816
	long		pv_used;	// 1069547520
	std::string	pv_uuid;	// D6IBDt-v35s-iehu-JPdK-U45M-DqBi-ZjkVP4

	long		pvseg_size;	// 1024
	long		pvseg_start;	// 255

	std::string	lv_name;	// myvol
	std::string	lv_type;	// mirror
	std::string	lv_attr;	// i
	std::string	lv_uuid;	// FLYSmN-CXwZ-O01d-cpmX-ZUbc-c8gm-o11d90

	long		vg_extent;	// 4194304
	std::string	vg_name;	// test_02
	long		vg_seqno;	// 3
	std::string	vg_uuid;	// xtV5An-Ua5C-iCba-JEpw-AlP0-7isO-iZCeXn
#endif

protected:
	LvmPartition (void);

private:

};


#endif // _LVM_PARTITION_H_

