/* Copyright (c) 2011 Richard Russon (FlatCap)
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

#include "container.h"
#include "partition.h"

class Filesystem : public Container
{
public:
	Filesystem (void);
	virtual ~Filesystem();

	virtual void Dump (int indent = 0);

				// container
	std::string	 type;	// name
	long long	 size;	// bytes_size
	long long	 used;	// bytes_used

	Partition	*part;

protected:

private:

};

#endif // _FILESYSTEM_H

