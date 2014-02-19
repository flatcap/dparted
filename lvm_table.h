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

#ifndef _LVM_TABLE_H_
#define _LVM_TABLE_H_

#include <string>
#include <memory>

#include "table.h"
#include "lvm_group.h"
#include "action.h"

class LvmTable;
class Visitor;

typedef std::shared_ptr<LvmTable> LvmTablePtr;

class LvmTable : public Table
{
public:
	virtual ~LvmTable() = default;
	static LvmTablePtr create (void);
	virtual bool accept (Visitor& v);

	static ContainerPtr probe (ContainerPtr& top_level, ContainerPtr& parent, unsigned char* buffer, int bufsize);

	virtual void add_child (ContainerPtr& child);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

public:
	// my volume
	std::string config;
	std::string pv_attr;

	int metadata_size = 0;

	LvmGroupPtr group;

protected:
	LvmTable (void);

private:

};


#endif // _LVM_TABLE_H_

