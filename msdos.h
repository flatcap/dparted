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

#ifndef _MSDOS_H
#define _MSDOS_H

#include <string>

#include "table.h"
#include "pointers.h"

class Visitor;

/**
 * struct partition
 */
struct partition {
	long start;
	long size;
	int type;
};

/**
 * class Msdos
 */
class Msdos : public Table
{
public:
	virtual ~Msdos() = default;
	static MsdosPtr create (void);
	virtual bool accept (Visitor& v);

	static ContainerPtr probe (ContainerPtr& top_level, ContainerPtr& parent, unsigned char* buffer, int bufsize);

public:
	//properties

protected:
	Msdos (void);

	virtual bool read_partition (unsigned char* buffer, int index, struct partition* part);
	virtual unsigned int read_table (unsigned char* buffer, int bufsize, long offset, std::vector<struct partition>& vp);
	virtual void read_chs (unsigned char* buffer, int* cylinder, int* head, int* sector);

private:

};


#endif // _MSDOS_H

