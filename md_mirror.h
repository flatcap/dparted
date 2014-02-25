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

#ifndef _MD_MIRROR_H_
#define _MD_MIRROR_H_

#include <memory>
#include <vector>

#include "md_volume.h"

class MdMirror;

typedef std::shared_ptr<MdMirror> MdMirrorPtr;

class MdMirror : public MdVolume
{
public:
	static MdMirrorPtr create (void);
	virtual ~MdMirror();
	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

public:
	//properties

protected:
	MdMirror (void);
};

#endif // _MD_MIRROR_H_

