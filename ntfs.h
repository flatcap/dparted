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

#ifndef _NTFS_H_
#define _NTFS_H_

#include <memory>
#include <string>
#include <vector>

#include "filesystem.h"

class Ntfs;

typedef std::shared_ptr<Ntfs> NtfsPtr;

class Ntfs : public Filesystem
{
public:
	static NtfsPtr create (void);
	virtual ~Ntfs();
	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

	static NtfsPtr get_ntfs (ContainerPtr parent, std::uint8_t* buffer, int bufsize);

public:
	//properties

protected:
	Ntfs (void);

	bool get_ntfs_usage (void);
	void get_ntfs_sb (ContainerPtr c);

	std::vector<std::string> more_props;
};

#endif // _NTFS_H_

