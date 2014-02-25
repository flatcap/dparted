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
#include <string>
#include <vector>

#include "volume.h"

class LvmVolume;

typedef std::shared_ptr<LvmVolume> LvmVolumePtr;

class LvmVolume : public Volume
{
public:
	static LvmVolumePtr create (void);
	virtual ~LvmVolume();
	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

	virtual void add_child (ContainerPtr& child);

	virtual ContainerPtr find (const std::string& uuid);

public:
	//properties
	std::string	lv_attr;
	long		kernel_major = -1;
	long		kernel_minor = -1;

	int		seg_count    = 0;
	int		stripes      = 0;
	int		stripesize   = 0;
	int		stripe_size  = 0;
	int		seg_start_pe = 0;

	std::string	mirror_log;

	std::vector<ContainerPtr> metadata;
	std::vector<ContainerPtr> subvols;

	ContainerPtr sibling;

protected:
	LvmVolume (void);

private:

};

#endif // _LVM_VOLUME_H_

