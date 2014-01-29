/* Copyright (c) 2014 Richard Russon (FlatCap)
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

#ifndef _BTRFS_H_
#define _BTRFS_H_

#include "filesystem.h"

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

protected:
	Btrfs (void);

	bool get_btrfs_usage (void);
	void get_btrfs_sb (ContainerPtr c);

	std::vector<std::string> more_props;
};


#endif // _BTRFS_H_

