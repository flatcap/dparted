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

#ifndef _LOOP_H_
#define _LOOP_H_

#include <string>
#include <queue>

#include "block.h"
#include "pointers.h"

/**
 * class Loop
 */
class Loop : public Block
{
public:
	static LoopPtr create (const std::string& losetup);

	virtual ~Loop() = default;

	// Backing file
	std::string	file_name;
	long		file_inode;
	int		file_major;
	int		file_minor;

	// Loop device
	int		loop_major;
	int		loop_minor;
	long		offset;
	long		sizelimit;
	bool		autoclear;
	bool		partscan;
	bool		read_only;
	bool		deleted;

	static bool losetup  (std::vector <std::string>& output, std::string device = std::string());
	static void discover (ContainerPtr& top_level, std::queue<ContainerPtr>& probe_queue);
	static void identify (ContainerPtr& top_level, const char* name, int fd, struct stat& st);

protected:
	Loop (void);

private:

};


#endif // _LOOP_H_

