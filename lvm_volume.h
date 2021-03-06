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

#ifndef _LVM_VOLUME_H_
#define _LVM_VOLUME_H_

#include <memory>
#include <set>
#include <string>
#include <vector>

#include "volume.h"

typedef std::shared_ptr<class LvmVolume> LvmVolumePtr;

class LvmVolume : public Volume
{
public:
	static LvmVolumePtr create (void);
	virtual ~LvmVolume();
	LvmVolume& operator= (const LvmVolume& c);
	LvmVolume& operator= (LvmVolume&& c);

	void swap (LvmVolume& c);
	friend void swap (LvmVolume& lhs, LvmVolume& rhs);

	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

	virtual bool __attribute__((warn_unused_result)) add_child (ContainerPtr child, bool probe);

	virtual ContainerPtr find (const std::string& uuid);

public:
	// properties
	std::string	lv_attr;
	std::uint64_t	seg_count    = 0;
	std::uint64_t	seg_start_pe = 0;
	std::uint64_t	stripes      = 0;
	std::uint64_t	stripe_size  = 0;

	std::string	mirror_log;		//XXX not declared

	std::vector<ContainerPtr> metadata;	//XXX not declared
	std::vector<ContainerPtr> subvols;	//XXX not declared

	ContainerPtr sibling;

protected:
	LvmVolume (void);
	LvmVolume (const LvmVolume& c);
	LvmVolume (LvmVolume&& c);

	virtual LvmVolume* clone (void);
};

#endif // _LVM_VOLUME_H_

