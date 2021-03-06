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

#include <memory>
#include <string>
#include <vector>

#include "table.h"
#include "lvm_group.h"

typedef std::shared_ptr<class LvmTable> LvmTablePtr;

class LvmTable : public Table
{
public:
	static LvmTablePtr create (void);
	virtual ~LvmTable();
	LvmTable& operator= (const LvmTable& c);
	LvmTable& operator= (LvmTable&& c);

	void swap (LvmTable& c);
	friend void swap (LvmTable& lhs, LvmTable& rhs);

	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

	static bool probe (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize);

	template<class T>
	bool __attribute__((warn_unused_result)) add_child (std::shared_ptr<T> child, bool probe);

	virtual bool __attribute__((warn_unused_result)) add_child (ContainerPtr child, bool probe);
	virtual bool set_alignment (std::uint64_t bytes);

public:
	// properties
	std::string config;	// my volume
	std::uint64_t metadata_size = 0;
	std::string pv_attr;

	LvmGroupPtr group;	//XXX not declared

protected:
	LvmTable (void);
	LvmTable (const LvmTable& c);
	LvmTable (LvmTable&& c);

	virtual LvmTable* clone (void);
};

template<class T>
bool
LvmTable::add_child (std::shared_ptr<T> child, bool probe)
{
	ContainerPtr c (child);
	return add_child (c, probe);
}

#endif // _LVM_TABLE_H_

