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

#include <string>
#include <memory>

#include "table.h"

class MdTable;
class Visitor;

typedef std::shared_ptr<MdTable> MdTablePtr;

/**
 * class MdTable
 */
class MdTable : public Table
{
public:
	virtual ~MdTable() = default;
	static MdTablePtr create (void);
	virtual bool accept (Visitor& v);

	static ContainerPtr probe (ContainerPtr& top_level, ContainerPtr& parent, unsigned char* buffer, int bufsize);

public:
	std::string	vol_uuid;
	std::string	vol_name;
	int		raid_type;
	int		raid_layout;
	int		raid_disks;
	int		chunk_size;
	long		chunks_used;
	long		data_offset;
	long		data_size;

protected:
	MdTable (void);

private:

};


#endif // _MD_TABLE_H_

