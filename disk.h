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

#ifndef _DISK_H_
#define _DISK_H_

#include <map>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include "block.h"

typedef std::shared_ptr<class Disk> DiskPtr;

class Disk : public Block
{
public:
	static DiskPtr create (void);
	static DiskPtr create (const std::string& lsblk);
	virtual ~Disk();
	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

	virtual std::uint64_t get_block_size (void);
	virtual std::uint64_t get_device_space (std::map<std::uint64_t, std::uint64_t>& spaces);

	static bool lsblk    (std::vector <std::string>& output, std::string device = std::string());
	static void discover (ContainerPtr& parent);
	static bool identify (ContainerPtr& parent, const std::string& name, int fd, struct stat& st);

public:
	// properties
	std::uint32_t	bios_cylinders = 0;	//XXX types
	std::uint32_t	bios_heads     = 0;
	std::uint32_t	bios_sectors   = 0;
	std::uint32_t	did            = 0;
	std::uint32_t	host           = 0;
	std::uint32_t	hw_cylinders   = 0;
	std::uint32_t	hw_heads       = 0;
	std::uint32_t	hw_sectors     = 0;
	bool		read_only      = false;
	// std::string	model;
	// std::string	path;
	// std::uint32_t	type;
	// std::uint64_t	sector_size;
	// std::uint64_t	phys_sector_size;
	// std::uint64_t	length;

	std::string mounts;	//XXX vector, move to partition, or filesystem

protected:
	Disk (void);
};

#endif // _DISK_H_

