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
#include <sstream>

#include "disk.h"
#include "utils.h"

/**
 * Disk
 */
Disk::Disk (void) :
	read_only (false),
	hw_cylinders (0),
	hw_heads (0),
	hw_sectors (0),
	bios_cylinders (0),
	bios_heads (0),
	bios_sectors (0),
	host (0),
	did (0)
{
	type = "disk";
}

/**
 * ~Disk
 */
Disk::~Disk()
{
}


/**
 * probe
 */
bool Disk::probe (const std::string &name, const struct stat &st)
{
	// for /dev/sda look at
	//	/sys/block/sda/size
	//	/sys/block/sda/device/vendor
	//	/sys/block/sda/device/model

	std::string readonly = read_file_line ("/sys/block/sda/ro");
	std::string size     = read_file_line ("/sys/block/sda/size");
	std::string model    = read_file_line ("/sys/block/sda/device/vendor");
	std::string vendor   = read_file_line ("/sys/block/sda/device/model");

	printf ("%s: %s\n", __FUNCTION__, model.c_str());

	return false;
}


/**
 * dump
 */
void Disk::dump (int indent /* = 0 */)
{
	std::string size = get_size (bytes_size);

	iprintf (indent,   "%s (%s)\n",  device.c_str(), size.c_str());
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

	Block::dump (indent);
}

/**
 * dump_csv
 */
void Disk::dump_csv (void)
{
	printf ("%s,%s,%s,%ld,%s,%lld,%lld,%lld\n",
		"Disk",
		device.c_str(),
		name.c_str(),
		block_size,
		uuid.c_str(),
		bytes_size,
		bytes_used,
		bytes_size - bytes_used);
	Block::dump_csv();
}

/**
 * dump_dot
 */
std::string Disk::dump_dot (void)
{
	std::ostringstream output;

	output << dump_table_header ("Disk", "red");

	output << Block::dump_dot();

	output << dump_row ("hw_cylinders",   hw_cylinders);
	output << dump_row ("hw_heads",       hw_heads);
	output << dump_row ("hw_sectors",     hw_sectors);
	output << dump_row ("read_only",      read_only);
	output << dump_row ("bios_cylinders", bios_cylinders);
	output << dump_row ("bios_heads",     bios_heads);
	output << dump_row ("bios_sectors",   bios_sectors);
	output << dump_row ("host",           host);
	output << dump_row ("did",            did);

	output << dump_table_footer();
	output << dump_dot_children();

	return output.str();
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
		return Block::find_device (dev);
	}

	return NULL;
}

