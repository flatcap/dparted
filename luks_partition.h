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

#ifndef _LUKS_PARTITION_H_
#define _LUKS_PARTITION_H_

#include <memory>
#include <vector>

#include "partition.h"

typedef std::shared_ptr<class LuksPartition> LuksPartitionPtr;

class LuksPartition : public Partition
{
public:
	static LuksPartitionPtr create (void);
	virtual ~LuksPartition();
	LuksPartition& operator= (const LuksPartition& c);
	LuksPartition& operator= (LuksPartition&& c);

	void swap (LuksPartition& c);
	friend void swap (LuksPartition& lhs, LuksPartition& rhs);

	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

public:
	// properties

protected:
	LuksPartition (void);
	LuksPartition (const LuksPartition& c);
	LuksPartition (LuksPartition&& c);

	virtual LuksPartition* clone (void);
};

#endif // _LUKS_PARTITION_H_

