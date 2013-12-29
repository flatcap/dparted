/* Copyright (c) 2013 Richard Russon (FlatCap)
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

#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include <string>
#include <memory>

#include "container.h"

class Filesystem;
class Visitor;

typedef std::shared_ptr<Filesystem> FilesystemPtr;

/**
 * class Filesystem
 */
class Filesystem : public Container
{
public:
	virtual ~Filesystem() = default;
	static FilesystemPtr create (void);
	virtual bool accept (Visitor& v);

	static FilesystemPtr probe (ContainerPtr& top_level, ContainerPtr& parent);

	virtual void mouse_event (void);

public:
	std::string	label;

protected:
	Filesystem (void);

	virtual std::string get_property (const std::string& propname);

private:
	long ext2_get_usage (void);

};


#endif // _FILESYSTEM_H

