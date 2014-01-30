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

#ifndef _NTFS_H_
#define _NTFS_H_

#include "filesystem.h"

class Ntfs;

typedef std::shared_ptr<Ntfs> NtfsPtr;

/**
 * class Ntfs
 */
class Ntfs : public Filesystem
{
public:
	virtual ~Ntfs() = default;
	static NtfsPtr create (void);

	static NtfsPtr get_ntfs (ContainerPtr parent, unsigned char* buffer, int bufsize);

protected:
	Ntfs (void);

	bool get_ntfs_usage (void);
	void get_ntfs_sb (ContainerPtr c);

	std::vector<std::string> more_props;
};


#endif // _NTFS_H_

