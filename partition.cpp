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


#include <stdio.h>
#include <string>

#include "partition.h"

Partition::Partition (void)
{
}

Partition::~Partition()
{
}

void Partition::Dump (int indent /* = 0 */)
{
	if (num < 0)
		return;

	std::string space = "                                                                ";

	space = space.substr (0, indent);

	printf ("%s\e[32mPartition\e[0m\n", space.c_str());
	printf ("%s    Type:       %s\n",   space.c_str(), type.c_str());
	printf ("%s    Filesystem: %s\n",   space.c_str(), fs.c_str());
	printf ("%s    Number:     %d\n",   space.c_str(), num);
	printf ("%s    Start:      %lld\n", space.c_str(), start);
	printf ("%s    Length:     %lld\n", space.c_str(), length);
	//printf ("%s    End:        %lld\n", space.c_str(), end);

	Container::Dump (indent);
}

