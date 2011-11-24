/* Copyright (c) 2011 Richard Russon (FlatCap)
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


#ifndef _DISK_H_
#define _DISK_H_

#include <string>

#include "container.h"

class Disk : public Container
{
public:
	Disk (void);
	virtual ~Disk();

	virtual void Dump (int indent = 0);

	std::string	model;
	std::string	path;
	int		type;
	long		sector_size;
	long		phys_sector_size;
	long long	length;
	bool		read_only;
	int		hw_cylinders;
	int		hw_heads;
	int		hw_sectors;
	int		bios_cylinders;
	int		bios_heads;
	int		bios_sectors;
	int		host;
	int		did;

protected:

private:

};

#endif // _DISK_H_
