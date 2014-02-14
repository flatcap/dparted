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

#ifndef _BTRFS_H_
#define _BTRFS_H_

#include "filesystem.h"
#include "action.h"

class Btrfs;

typedef std::shared_ptr<Btrfs> BtrfsPtr;

/**
 * class Btrfs
 */
class Btrfs : public Filesystem
{
public:
	virtual ~Btrfs() = default;
	static BtrfsPtr create (void);

	static BtrfsPtr get_btrfs (ContainerPtr parent, unsigned char* buffer, int bufsize);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

public:
	//properties

protected:
	Btrfs (void);

	bool get_btrfs_usage (void);
	void get_btrfs_sb (ContainerPtr c);

	std::vector<std::string> more_props;
};


#endif // _BTRFS_H_

