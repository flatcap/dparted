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
#include <typeinfo>

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
	//prev (NULL),
	//next (NULL)
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
	for (std::vector<Container*>::iterator i = children.begin(); i != children.end(); i++) {
		(*i)->dump (indent + 8);
	}
}

/**
 * dump_csv
 */
void Container::dump_csv (void)
{
	for (std::vector<Container*>::iterator i = children.begin(); i != children.end(); i++) {
		(*i)->dump_csv();
	}
}


/**
 * add_child
 */
void Container::add_child (Container *child)
{
	/* Check:
	 *	available space
	 *	alignment type
	 *	size (restrictions)
	 *	valid type within this parent
	 */
#if 0
	if (children.size() > 0) {
		Container *last = children.back();

		last->next = child;
		child->prev = last;
	}
#endif
	children.push_back (child);
}

/**
 * delete_child
 */
void Container::delete_child (Container *child)
{
}

/**
 * move_child
 */
void Container::move_child (Container *child, long long offset, long long size)
{
}


/**
 * get_block_size
 */
long Container::get_block_size (void)
{
	if (block_size > 0)
		return block_size;

	if (parent)
		return parent->get_block_size();

	return -1;
}

/**
 * get_device_name
 */
std::string Container::get_device_name (void)
{
	return device;
}

/**
 * get_device_offset
 */
long long Container::get_device_offset (void)
{
	return device_offset;
}

/**
 * get_device_space
 */
unsigned int Container::get_device_space (std::map<long long, long long> &spaces)
{
	spaces.clear();

	//spaces[offset] = size
	//spaces[0] = 123;

	return spaces.size();;
}


/**
 * get_size_total
 */
long long Container::get_size_total (void)
{
	return bytes_size;
}

/**
 * get_size_used
 */
long long Container::get_size_used (void)
{
	return bytes_used;
}

/**
 * get_size_free
 */
long long Container::get_size_free (void)
{
	return bytes_size - bytes_used;
}


/**
 * find_device
 */
Container * Container::find_device (const std::string &dev)
{
	return NULL;
}

/**
 * find_uuid
 */
Container * Container::find_uuid (const std::string &uuid)
{
	return NULL;
}

