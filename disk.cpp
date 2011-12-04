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
#include "utils.h"

/**
 * Disk
 */
Disk::Disk (void)
{
	type = "\e[31mdisk\e[0m";
}

/**
 * ~Disk
 */
Disk::~Disk()
{
}


/**
 * dump
 */
void Disk::dump (int indent /* = 0 */)
{
	std::string size = get_size (bytes_size);

	iprintf (indent,   "\e[31m%s\e[0m (%s)\n",  device.c_str(), size.c_str());
	iprintf (indent+8, "Model:  %s\n",  name.c_str());
	iprintf (indent+8, "Sector: %ld\n", block_size);

	//iprintf (indent+8, "Path:   %s\n",   path.c_str());
	//iprintf (indent+8, "Type:   %d\n",   type);
	//iprintf (indent+8, "Length: %s\n", size.c_str());

	//iprintf (indent+8, "Physical Sector Size: %ld\n",        phys_sector_size);
	//iprintf (indent+8, "Read only:            %d\n",         read_only);
	//iprintf (indent+8, "Hardware (CHS):       (%d,%d,%d)\n", hw_cylinders,   hw_heads,   hw_sectors);
	//iprintf (indent+8, "BIOS     (CHS):       (%d,%d,%d)\n", bios_cylinders, bios_heads, bios_sectors);
	//iprintf (indent+8, "Host:                 %d\n",         host);
	//iprintf (indent+8, "DID:                  %d\n",         did);

	Container::dump (indent);
}

/**
 * dump_csv
 */
void Disk::dump_csv (void)
{
	printf ("%s,%s,%s,%ld,%s,%s,%lld,%lld,%lld\n",
		"Disk",
		device.c_str(),
		name.c_str(),
		block_size,
		label.c_str(),
		uuid.c_str(),
		bytes_size,
		bytes_used,
		bytes_size - bytes_used);
	Container::dump_csv();
}


/**
 * get_block_size
 */
long Disk::get_block_size (void)
{
	return 0;
}

#if 0
/**
 * get_device_name
 */
std::string Disk::get_device_name (void)
{
	return device;
}

/**
 * get_device_offset
 */
long long Disk::get_device_offset (void)
{
	return 0;
}

#endif
/**
 * get_device_space
 */
unsigned int Disk::get_device_space (std::map<long long, long long> &spaces)
{
	return 0;
}


/**
 * find_device
 */
Container * Disk::find_device (const std::string &dev)
{
	// does it sound like one of my children?  /dev/sdaX, /dev/sdaXX
	unsigned int dev_len = device.length();

	// iterate through my children
	if (device.compare (0, dev_len, dev, 0, dev_len) == 0) {
		//printf ("similar\n");
		return Container::find_device (dev);
	}

	return NULL;
}

