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


#ifndef _CONTAINER_H_
#define _CONTAINER_H_

#include <string>
#include <vector>

class Container
{
public:
	Container (void);
	virtual ~Container();

	virtual void dump (int indent = 0);
	virtual void dump_csv (void);

	virtual void add_child (Container *child);

	int		 type;

	std::string	 name;
	std::string	 label;
	std::string	 device;
	std::string	 uuid;

	long		 block_size;
	long long	 bytes_size;
	long long	 bytes_used;

	Container	*parent;
	std::vector<Container*> children;
	std::vector<long long>  offsets;

	Container	*prev;	// Siblings
	Container	*next;

protected:

private:

};

#endif // _CONTAINER_H_

