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

#include "piece.h"
#include "action.h"

class LvmPartition;
class Visitor;

typedef std::shared_ptr<LvmPartition> LvmPartitionPtr;

class LvmPartition : public Piece
{
public:
	virtual ~LvmPartition() = default;
	static LvmPartitionPtr create (void);
	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

public:
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

