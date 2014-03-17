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

class Disk;

typedef std::shared_ptr<Disk> DiskPtr;

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
	virtual std::uint64_t get_device_space (std::map<std::uint64_t,std::uint64_t>& spaces);

	static unsigned int find_devices (ContainerPtr& list);

	static bool lsblk    (std::vector <std::string>& output, std::string device = std::string());
	static void discover (ContainerPtr& top_level, std::queue<ContainerPtr>& probe_queue);
	static void identify (ContainerPtr& top_level, const char* name, int fd, struct stat& st);

public:
	//properties
	int		bios_cylinders = 0;	//XXX types
	int		bios_heads     = 0;
	int		bios_sectors   = 0;
	int		did            = 0;
	int		host           = 0;
	int		hw_cylinders   = 0;
	int		hw_heads       = 0;
	int		hw_sectors     = 0;
	bool		read_only      = false;
	//std::string	model;
	//std::string	path;
	//int		type;
	//long		sector_size;
	//long		phys_sector_size;
	//long	length;

	std::string mounts;	//XXX vector

protected:
	Disk (void);

private:

};

#endif // _DISK_H_

