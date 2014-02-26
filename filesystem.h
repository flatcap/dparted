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

#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include <memory>
#include <string>
#include <vector>

#include "container.h"

class Filesystem;

typedef std::shared_ptr<Filesystem> FilesystemPtr;

/**
 * class Filesystem - The actual object containing your files
 */
class Filesystem : public Container
{
public:
	static FilesystemPtr create (void);
	virtual ~Filesystem();
	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

	static FilesystemPtr probe (ContainerPtr& top_level, ContainerPtr& parent);
	bool get_mounted_usage (ContainerPtr parent);

public:
	//properties

protected:
	Filesystem (void);

private:
	long ext2_get_usage (void);

};

#endif // _FILESYSTEM_H

