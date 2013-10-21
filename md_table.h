/* Copyright (c) 2013 Richard Russon (FlatCap)
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Library General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _MD_TABLE_H_
#define _MD_TABLE_H_

#include <string>

#include "table.h"

/**
 * class MdTable
 */
class MdTable : public Table
{
public:
	MdTable (void);
	virtual ~MdTable();

	static DPContainer * probe (DPContainer &top_level, DPContainer *parent, unsigned char *buffer, int bufsize);

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
private:

};


#endif // _MD_TABLE_H_

