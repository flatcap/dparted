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

#ifndef _MD_PARTITION_H_
#define _MD_PARTITION_H_

#include <memory>
#include <vector>

#include "partition.h"

typedef std::shared_ptr<class MdPartition> MdPartitionPtr;

class MdPartition : public Partition
{
public:
	static MdPartitionPtr create (void);
	virtual ~MdPartition();
	MdPartition& operator= (const MdPartition& c);
	MdPartition& operator= (MdPartition&& c);

	void swap (MdPartition& c);
	friend void swap (MdPartition& lhs, MdPartition& rhs);

	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

public:
	// properties

protected:
	MdPartition (void);
	MdPartition (const MdPartition& c);
	MdPartition (MdPartition&& c);

	virtual MdPartition* clone (void);
};

#endif // _MD_PARTITION_H_

