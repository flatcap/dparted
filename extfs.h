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

#include "filesystem.h"

class ExtFs;

typedef std::shared_ptr<ExtFs> ExtFsPtr;

/**
 * class ExtFs
 */
class ExtFs : public Filesystem
{
public:
	virtual ~ExtFs() = default;
	static ExtFsPtr create (void);

	void get_ext_sb (ContainerPtr parent);
	static ExtFsPtr get_ext2 (ContainerPtr parent, unsigned char* buffer, int bufsize);
	static ExtFsPtr get_ext3 (ContainerPtr parent, unsigned char* buffer, int bufsize);
	static ExtFsPtr get_ext4 (ContainerPtr parent, unsigned char* buffer, int bufsize);

protected:
	ExtFs (void);

	std::vector<std::string> more_props;
};


#endif // _EXTFS_H_

