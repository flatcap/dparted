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


#include <stdio.h>
#include <string>

#include "disk.h"

Disk::Disk (void)
{
}

Disk::~Disk()
{
}

void Disk::Dump (int indent /* = 0 */)
{
	std::string space = "                                                                ";

	space = space.substr (0, indent);

	printf ("%s\e[31m%s\e[0m\n",  space.c_str(), device.c_str());
	printf ("%s\tModel:  %s\n",   space.c_str(), model.c_str());
	//printf ("%s\tPath:   %s\n",   space.c_str(), path.c_str());
	//printf ("%s\tType:   %d\n",   space.c_str(), type);
	printf ("%s\tSector: %ld\n",  space.c_str(), sector_size); // sector size
	printf ("%s\tLength: %lld\n", space.c_str(), length);

	//printf ("%s\tPhysical Sector Size: %ld\n",        space.c_str(), phys_sector_size);
	//printf ("%s\tRead only:            %d\n",         space.c_str(), read_only);
	//printf ("%s\tHardware (CHS):       (%d,%d,%d)\n", space.c_str(), hw_cylinders,   hw_heads,   hw_sectors);
	//printf ("%s\tBIOS     (CHS):       (%d,%d,%d)\n", space.c_str(), bios_cylinders, bios_heads, bios_sectors);
	//printf ("%s\tHost:                 %d\n",         space.c_str(), host);
	//printf ("%s\tDID:                  %d\n",         space.c_str(), did);

	Container::Dump (indent);
}

