/* Copyright (c) 2011-2012 Richard Russon (FlatCap)
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
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <string>
#include <sstream>

#include "loop.h"
#include "main.h"

/**
 * Loop
 */
Loop::Loop (void)
{
	name = "loop";
	type = "loop";
}

/**
 * ~Loop
 */
Loop::~Loop()
{
}


/**
 * probe
 */
bool Loop::probe (const std::string &name, int fd, struct stat &st, Container &list)
{
	Loop *l = NULL;

	l = new Loop;

	l->device        = name;
	l->device_offset = 0;
	l->bytes_size    = st.st_size;
	l->bytes_used    = 0;

	list.add_child (l);
	queue_add_probe (l);	// queue the container for action

	return true;
}

