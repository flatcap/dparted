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

#include <memory>
#include <string>
#include <vector>

#include "filesystem.h"

typedef std::shared_ptr<class Btrfs> BtrfsPtr;

class Btrfs : public Filesystem
{
public:
	static BtrfsPtr create (void);
	virtual ~Btrfs();
	Btrfs& operator= (const Btrfs& c);
	Btrfs& operator= (Btrfs&& c);

	void swap (Btrfs& c);
	friend void swap (Btrfs& lhs, Btrfs& rhs);

	BtrfsPtr copy (void);

	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

	static BtrfsPtr get_btrfs (ContainerPtr parent, std::uint8_t* buffer, int bufsize);

public:
	// properties

protected:
	Btrfs (void);
	Btrfs (const Btrfs& c);
	Btrfs (Btrfs&& c);

	virtual Btrfs* clone (void);

	bool get_btrfs_usage (void);
	void get_btrfs_sb (ContainerPtr c);

	std::vector<std::string> more_props;
};

#endif // _BTRFS_H_

