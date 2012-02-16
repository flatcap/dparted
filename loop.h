/* Copyright (c) 2012 Richard Russon (FlatCap)
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


#ifndef _LOOP_H_
#define _LOOP_H_

#include <string>

#include "block.h"

class Container;

class Loop : public Block
{
public:
	Loop (void);
	virtual ~Loop();

	static bool probe (const std::string &name, int fd, struct stat &st, Container &list);
	static unsigned int find_devices (std::vector<Container *> &list);

	virtual std::string dump_dot (void);

protected:
	// these refer to the backing file, not the loop device itself
	int kernel_major;
	int kernel_minor;
	long inode;
	std::string file;

private:

};

#endif // _LOOP_H_

