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

#ifndef _EXTFS_H_
#define _EXTFS_H_

#include <memory>
#include <string>
#include <vector>

#include "filesystem.h"

class Extfs;

typedef std::shared_ptr<Extfs> ExtfsPtr;

class Extfs : public Filesystem
{
public:
	static ExtfsPtr create (void);
	virtual ~Extfs();
	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

	void get_ext_sb (ContainerPtr parent);
	static ExtfsPtr get_ext2 (ContainerPtr parent, std::uint8_t* buffer, std::uint64_t bufsize);
	static ExtfsPtr get_ext3 (ContainerPtr parent, std::uint8_t* buffer, std::uint64_t bufsize);
	static ExtfsPtr get_ext4 (ContainerPtr parent, std::uint8_t* buffer, std::uint64_t bufsize);

public:
	//properties

protected:
	Extfs (void);

	std::vector<std::string> more_props;
};

#endif // _EXTFS_H_

