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

typedef std::shared_ptr<class Loop> LoopPtr;

class Loop : public Block
{
public:
	static LoopPtr create (const std::string& losetup);
	static LoopPtr create (void);
	virtual ~Loop();
	Loop& operator= (const Loop& c);
	Loop& operator= (Loop&& c);

	void swap (Loop& c);
	friend void swap (Loop& lhs, Loop& rhs);

	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

	static bool losetup  (std::vector <std::string>& output, std::string device = std::string());
	static void discover (ContainerPtr& parent);
	static bool identify (ContainerPtr& parent, const std::string& name, int fd, struct stat& st);

	virtual bool can_delete (QuestionPtr q);
	virtual bool is_resizeable (void);

public:
	// properties
	bool		autoclear  = false;
	bool		deleted    = false;
	std::uint64_t	file_inode = 0;
	dev_t		file_major = 0;
	dev_t		file_minor = 0;
	std::string	file_name;		// Backing file
	std::uint64_t	offset     = 0;
	bool		partscan   = false;
	bool		read_only  = false;
	std::uint64_t	sizelimit  = 0;

protected:
	Loop (void);
	Loop (const Loop& c);
	Loop (Loop&& c);

	virtual Loop* clone (void);

	std::string get_file_major_minor (void);
	std::string get_file_name_short (void);
	std::string get_flags (void);
};

#endif // _LOOP_H_

