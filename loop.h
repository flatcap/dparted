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

#ifndef _LOOP_H_
#define _LOOP_H_

#include <memory>
#include <queue>
#include <string>
#include <vector>

#include "block.h"

class Loop;

typedef std::shared_ptr<Loop> LoopPtr;

class Loop : public Block
{
public:
	static LoopPtr create (const std::string& losetup);
	virtual ~Loop();
	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

	static bool losetup  (std::vector <std::string>& output, std::string device = std::string());
	static void discover (ContainerPtr& top_level, std::queue<ContainerPtr>& probe_queue);
	static void identify (ContainerPtr& top_level, const char* name, int fd, struct stat& st);

public:
	//properties
	std::string	file_name;		// Backing file
	long		file_inode = 0;
	int		file_major = 0;
	int		file_minor = 0;

	int		loop_major = 0;		// Loop device
	int		loop_minor = 0;
	long		offset     = 0;
	long		sizelimit  = 0;
	bool		autoclear  = false;
	bool		partscan   = false;
	bool		read_only  = false;
	bool		deleted    = false;

protected:
	Loop (void);

private:

};

#endif // _LOOP_H_

