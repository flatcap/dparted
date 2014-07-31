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

#ifndef _FILE_H_
#define _FILE_H_

#include <memory>
#include <queue>
#include <string>
#include <vector>

#include "container.h"

typedef std::shared_ptr<class File> FilePtr;

class File : public Container
{
public:
	static FilePtr create (void);
	virtual ~File();
	File& operator= (const File& c);
	File& operator= (File&& c);

	void swap (File& c);
	friend void swap (File& lhs, File& rhs);

	FilePtr copy (void);

	virtual bool accept (Visitor& v);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

	static void discover (ContainerPtr& parent);
	static bool identify (ContainerPtr& parent, const std::string& name, int fd, struct stat& st);

public:
	// properties

protected:
	File (void);
	File (const File& c);
	File (File&& c);

	virtual File* clone (void);
};

#endif // _FILE_H_

