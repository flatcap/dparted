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

#include "container.h"
#include "utils.h"

/**
 * Container
 */
Container::Container (void) :
	type (0),
	block_size (-1),
	bytes_size (-1),
	bytes_used (-1),
	parent (NULL)
{
}

/**
 * ~Container
 */
Container::~Container()
{
	for (std::vector<Container*>::iterator i = children.begin(); i != children.end(); i++) {
		delete *i;
	}
}

/**
 * dump
 */
void Container::dump (int indent /* = 0 */)
{
	unsigned int i;

	//iprintf (indent+8, "\e[07;31mParent = %p\e[0m\n", parent);
	for (i = 0; i < children.size(); i++) {
		children[i]->dump (indent + 8);
	}
}

