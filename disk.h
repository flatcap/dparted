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

#ifndef _DISK_H_
#define _DISK_H_

#include <map>
#include <string>

#include "block.h"

/**
 * class Disk
 */
class Disk : public Block
{
public:
	Disk (void);
	virtual ~Disk();

	virtual std::string dump_dot (void);

	virtual long          get_block_size (void);
	//virtual std::string   get_device_name (void);
	//virtual long long     get_parent_offset (void);
	virtual unsigned int  get_device_space (std::map<long long, long long> &spaces);

	virtual DPContainer * find_device (const std::string &dev);

	//std::string	model;
	//std::string	path;
	//int		type;
	//long		sector_size;
	//long		phys_sector_size;
	//long long	length;
	bool		read_only;
	int		hw_cylinders;
	int		hw_heads;
	int		hw_sectors;
	int		bios_cylinders;
	int		bios_heads;
	int		bios_sectors;
	int		host;
	int		did;

	static unsigned int find_devices (DPContainer &list);

protected:
	int kernel_major;
	int kernel_minor;
	std::string mounts;	//XXX vector

private:

};

#endif // _DISK_H_

