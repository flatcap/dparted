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

#ifndef _LVM_MIRROR_H_
#define _LVM_MIRROR_H_

#include <memory>
#include <vector>

#include "lvm_volume.h"

typedef std::shared_ptr<class LvmMirror> LvmMirrorPtr;

class LvmMirror : public LvmVolume
{
public:
	static LvmMirrorPtr create (void);
	virtual ~LvmMirror();
	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

	template<class T>
	void add_child (std::shared_ptr<T>& child, bool probe, const char* description)
	{
		ContainerPtr c (child);
		add_child (c, probe, description);
	}

	virtual void add_child    (ContainerPtr& child, bool probe, const char* description);
	virtual void delete_child (ContainerPtr& child);

public:
	// properties

protected:
	LvmMirror (void);
#if 0
	std::vector<ContainerPtr> mirrors;
	ContainerPtr log;
#endif
};

#endif // _LVM_MIRROR_H_

