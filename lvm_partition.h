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

#ifndef _LVM_PARTITION_H_
#define _LVM_PARTITION_H_

#include <memory>
#include <string>
#include <vector>

#include "partition.h"

typedef std::shared_ptr<class LvmPartition> LvmPartitionPtr;

class LvmPartition : public Partition
{
public:
	static LvmPartitionPtr create (void);
	virtual ~LvmPartition();
	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

public:
	// properties
#if 0
	std::uint64_t	dev_size;	//XXX not declared
	std::uint64_t	pe_start;	// 1048576
	std::string	pv_attr;	// a--
	std::uint64_t	pv_count;	// 8
	std::uint64_t	pv_free;	// 4294967296
	std::string	pv_name;	// /dev/loop2
	std::uint64_t	pv_pe_alloc;	// 255
	std::uint64_t	pv_pe_count;	// 1279
	std::uint64_t	pv_size;	// 5364514816
	std::uint64_t	pv_used;	// 1069547520
	std::string	pv_uuid;	// D6IBDt-v35s-iehu-JPdK-U45M-DqBi-ZjkVP4

	std::uint64_t	pvseg_size;	// 1024
	std::uint64_t	pvseg_start;	// 255

	std::string	lv_name;	// myvol
	std::string	lv_type;	// mirror
	std::string	lv_attr;	// i
	std::string	lv_uuid;	// FLYSmN-CXwZ-O01d-cpmX-ZUbc-c8gm-o11d90

	std::uint64_t	vg_extent;	// 4194304
	std::string	vg_name;	// test_02
	std::uint64_t	vg_seqno;	// 3
	std::string	vg_uuid;	// xtV5An-Ua5C-iCba-JEpw-AlP0-7isO-iZCeXn
#endif

protected:
	LvmPartition (void);
};

#endif // _LVM_PARTITION_H_

