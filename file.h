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

#include <string>
#include <queue>
#include <memory>

#include "block.h"
#include "action.h"

class File;
class Visitor;

typedef std::shared_ptr<File> FilePtr;

/**
 * class File
 */
class File : public Block
{
public:
	virtual ~File() = default;
	static FilePtr create (void);
	virtual bool accept (Visitor& v);

	static void discover (ContainerPtr& top_level, std::queue<ContainerPtr>& probe_queue);
	static void identify (ContainerPtr& top_level, const char* name, int fd, struct stat& st);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

public:
	//properties

protected:
	File (void);

private:

};


#endif // _FILE_H_

