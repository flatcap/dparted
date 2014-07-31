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

#ifndef _MD_RAID_H_
#define _MD_RAID_H_

#include <memory>
#include <vector>

#include "md_volume.h"

typedef std::shared_ptr<class MdRaid> MdRaidPtr;

class MdRaid : public MdVolume
{
public:
	static MdRaidPtr create (void);
	virtual ~MdRaid();
	MdRaid& operator= (const MdRaid& c);
	MdRaid& operator= (MdRaid&& c);

	void swap (MdRaid& c);
	friend void swap (MdRaid& lhs, MdRaid& rhs);

	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

public:
	// properties

protected:
	MdRaid (void);
	MdRaid (const MdRaid& c);
	MdRaid (MdRaid&& c);

	virtual MdRaid* clone (void);
};

#endif // _MD_RAID_H_

