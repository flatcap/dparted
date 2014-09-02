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

#ifndef _TABLE_H_
#define _TABLE_H_

#include <memory>
#include <string>
#include <vector>

#include "container.h"

typedef std::shared_ptr<class Table> TablePtr;

/**
 * class Table - Divides a Block into Partition objects
 */
class Table : public Container
{
public:
	static TablePtr create (void);
	virtual ~Table();
	Table& operator= (const Table& c);
	Table& operator= (Table&& c);

	void swap (Table& c);
	friend void swap (Table& lhs, Table& rhs);

	virtual bool accept (Visitor& v);

	virtual bool __attribute__((warn_unused_result)) add_child    (ContainerPtr child, bool probe);
	virtual bool __attribute__((warn_unused_result)) delete_child (ContainerPtr child);
	virtual bool __attribute__((warn_unused_result)) move_child   (ContainerPtr child, std::uint64_t offset, std::uint64_t size);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

	static bool probe (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize);

	virtual bool set_alignment (std::uint64_t bytes);

public:
	// properties

protected:
	Table (void);
	Table (const Table& c);
	Table (Table&& c);

	virtual Table* clone (void);

	std::uint64_t fill_space (void);
};

#endif // _TABLE_H_

