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

#ifndef _MD_LINEAR_H_
#define _MD_LINEAR_H_

#include <memory>
#include <vector>

#include "md_volume.h"

typedef std::shared_ptr<class MdLinear> MdLinearPtr;

class MdLinear : public MdVolume
{
public:
	static MdLinearPtr create (void);
	virtual ~MdLinear();
	MdLinear& operator= (const MdLinear& c);
	MdLinear& operator= (MdLinear&& c);

	void swap (MdLinear& c);
	friend void swap (MdLinear& lhs, MdLinear& rhs);

	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

public:
	// properties

protected:
	MdLinear (void);
	MdLinear (const MdLinear& c);
	MdLinear (MdLinear&& c);

	virtual MdLinear* clone (void);
};

#endif // _MD_LINEAR_H_

