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

#ifndef _LVM_RAID_H_
#define _LVM_RAID_H_

#include <memory>
#include <vector>

#include "lvm_volume.h"

typedef std::shared_ptr<class LvmRaid> LvmRaidPtr;

class LvmRaid : public LvmVolume
{
public:
	static LvmRaidPtr create (void);
	virtual ~LvmRaid();
	LvmRaid& operator= (const LvmRaid& c);
	LvmRaid& operator= (LvmRaid&& c);

	void swap (LvmRaid& c);
	friend void swap (LvmRaid& lhs, LvmRaid& rhs);

	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

public:
	// properties

protected:
	LvmRaid (void);
	LvmRaid (const LvmRaid& c);
	LvmRaid (LvmRaid&& c);

	virtual LvmRaid* clone (void);
};

#endif // _LVM_RAID_H_

