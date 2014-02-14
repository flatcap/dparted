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

#ifndef _MD_GROUP_H_
#define _MD_GROUP_H_

#include <memory>

#include "whole.h"
#include "action.h"

class MdGroup;
class MdTable;
class Visitor;

typedef std::shared_ptr<MdGroup> MdGroupPtr;

/**
 * class MdGroup
 */
class MdGroup : public Whole
{
public:
	virtual ~MdGroup() = default;
	static MdGroupPtr create (void);
	virtual bool accept (Visitor& v);

	static void discover (ContainerPtr& top_level);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

public:
	//properties

protected:
	MdGroup (void);

private:

};


#endif // _MD_GROUP_H_

