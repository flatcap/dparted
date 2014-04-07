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

#ifndef _MD_TABLE_H_
#define _MD_TABLE_H_

#include <memory>
#include <string>
#include <vector>

#include "table.h"

class MdTable;

typedef std::shared_ptr<MdTable> MdTablePtr;

class MdTable : public Table
{
public:
	static MdTablePtr create (void);
	virtual ~MdTable();
	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

	static bool probe (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize);

public:
	//properties
	std::uint64_t	chunk_size;
	std::uint64_t	chunks_used;
	std::uint64_t	data_offset;
	std::uint64_t	data_size;
	std::uint32_t	raid_disks;
	std::uint32_t	raid_layout;
	std::uint32_t	raid_type;
	std::string	vol_name;
	std::string	vol_uuid;

protected:
	MdTable (void);

private:

};

#endif // _MD_TABLE_H_

