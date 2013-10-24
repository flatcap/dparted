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

#ifndef _LVM_TABLE_H_
#define _LVM_TABLE_H_

#include <string>

#include "table.h"

class LvmGroup;

/**
 * class LvmTable
 */
class LvmTable : public Table
{
public:
	LvmTable (void);
	virtual ~LvmTable();

	static DPContainer * probe (DPContainer &top_level, DPContainer *parent, unsigned char *buffer, int bufsize);

	// my volume
	std::string vol_name;
	std::string config;

	int seq_num;

	int metadata_size;

	LvmGroup *group;
protected:

private:

};


#endif // _LVM_TABLE_H_

