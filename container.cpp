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

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iterator>
#include <cstdio>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <typeinfo>

#include "container.h"
#include "log.h"
#include "utils.h"
#include "leak.h"
#include "log_trace.h"

/**
 * DPContainer
 */
DPContainer::DPContainer (void) :
	parent_offset (0),
	block_size (0),
	bytes_size (0),
	bytes_used (0),
	whole (NULL),
	parent (NULL),
	fd (NULL),		//XXX WHAT?
	ref_count (0)
{
	declare ("container");
}

/**
 * ~DPContainer
 */
DPContainer::~DPContainer()
{
	for (auto i : children) {
		i->unref();
	}

	if (fd) {		//XXX what? >=0 is valid
		fclose (fd);
	}
}


/**
 * operator new
 */
void * DPContainer::operator new (size_t size)
{
	DPContainer *c = (DPContainer*) malloc (size);

	//log_info ("new object %p\n", (void*) c);

	return object_track (c);
}

/**
 * operator delete
 */
void DPContainer::operator delete (void *ptr)
{
	if (!ptr)
		return;

	DPContainer *c = (DPContainer *) (ptr);
	if (c->ref_count != 0)
		log_error ("REF COUNT = %d\n", c->ref_count);

	//log_info ("deleted object %p\n", ptr);
	object_untrack (c);
	free (ptr);
}

/**
 * add_child
 */
void DPContainer::add_child (DPContainer *child)
{
	/* Check:
	 *	available space
	 *	alignment type
	 *	size (restrictions)
	 *	valid type within this parent
	 */
#if 0
	if (children.size() > 0) {
		DPContainer *last = children.back();

		last->next = child;
		child->prev = last;
	}
#endif
	bytes_used += child->bytes_size;

	bool inserted = false;

	for (auto i = children.begin(); i != children.end(); i++) {
		if ((*i)->parent_offset > child->parent_offset) {
			children.insert (i, child);
			inserted = true;
			break;
		}
	}

	if (!inserted) {
		children.push_back (child);
	}

	log_debug ("insert: %s (%s)\n", this->name.c_str(), child->name.c_str());

	child->ref();
	child->parent = this;
}

/**
 * delete_child
 */
void DPContainer::delete_child (DPContainer *child)
{
}

/**
 * move_child
 */
void DPContainer::move_child (DPContainer *child, long long offset, long long size)
{
}


/**
 * get_block_size
 */
long DPContainer::get_block_size (void)
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
std::string DPContainer::get_device_name (void)
{
	//log_debug ("i am %s\n", typeid (*this).name());
	if (device.length() > 0)
		return device;
	else if (parent)
		return parent->get_device_name();
	else
		return "UNKNOWN";
}

/**
 * get_parent_offset
 */
long long DPContainer::get_parent_offset (void)
{
	return parent_offset;
}

/**
 * get_device_space
 */
unsigned int DPContainer::get_device_space (std::map<long long, long long> &spaces)
{
	spaces.clear();

	//spaces[offset] = size
	//spaces[0] = 123;

	return spaces.size();;
}


/**
 * get_size_total
 */
long long DPContainer::get_size_total (void)
{
	return bytes_size;
}

/**
 * get_size_used
 */
long long DPContainer::get_size_used (void)
{
	return bytes_used;
}

/**
 * get_size_free
 */
long long DPContainer::get_size_free (void)
{
	return bytes_size - bytes_used;
}


/**
 * find_device
 */
DPContainer * DPContainer::find_device (const std::string &dev)
{
	DPContainer *match = NULL;

	// am *I* the device?
	//log_debug ("Me? %s %s\n", device.c_str(), dev.c_str());
	if (dev == device)
		return this;

	for (auto i : children) {
		//log_debug ("child %p (%s)\n", i, i->device.c_str());
		match = i->find_device (dev);
		if (match) {
			//log_debug ("MATCHES! %s (%s)\n", match->type.back().c_str(), match->name.c_str());
			break;
		}
	}

	return match;
}

/**
 * find_uuid
 */
DPContainer * DPContainer::find_uuid (const std::string &uuid)
{
	for (auto i : children) {
		if (i->uuid == uuid) {
			return i;
		}
	}

	return NULL;
}

/**
 * find_name
 */
DPContainer * DPContainer::find_name (const std::string &name)
{
	for (auto i : children) {
		if (i->name == name) {
			return i;
		}
	}

	return NULL;
}


/**
 * open_device
 */
FILE * DPContainer::open_device (void)
{
	// flags? ro, rw
	if (fd)
		return fd;

	if (device.empty()) {
		if (parent) {
			fd = parent->open_device();
		}
		return fd;
	}

	fd = fopen (device.c_str(), "re");	// read, close on exec
	if (fd == NULL) {
		log_error ("failed to open device %s\n", device.c_str());
	}

	log_info ("OPEN %s = %p\n", device.c_str(), (void*) fd);
	return fd;
}

/**
 * read_data
 */
int DPContainer::read_data (long long offset, long long size, unsigned char *buffer)
{
	// if offset >= 0, seek there
	long long current = -1;
	long long bytes = 0;

	if (!buffer)
		return -1;

	if (fd == NULL) {
		if (parent) {
			return parent->read_data (offset + parent_offset, size, buffer);
		} else {
			return -1;
		}
	}

	if (offset >= 0) {
		current = fseek (fd, offset, SEEK_SET);
	} else {
		current = fseek (fd, 0, SEEK_CUR);
	}

	if (current < 0) {
		log_error ("seek to %lld failed on %s (%p)\n", offset, device.c_str(), (void*) fd);
		perror ("seek");
		return -1;
	}

	if ((current + size) > bytes_size)
		return -1;

	bytes = fread (buffer, size, 1, fd);
	bytes *= size;
	std::string s = get_size (current);
	log_info ("READ: device %s (%p), offset %lld (%s), size %lld = %lld\n", device.c_str(), (void*) fd, current, s.c_str(), size, bytes);

	return bytes;
}


/**
 * operator<<
 */
std::ostream& operator<< (std::ostream &stream, const DPContainer &c)
{
	stream << c.type.back() << ", " << c.name << ", " << c.device << "(off " << c.parent_offset << "), " << c.children.size();
	return stream;
}

/**
 * operator<<
 */
std::ostream& operator<< (std::ostream &stream, const DPContainer *c)
{
	if (c)
		return operator<< (stream, *c);
	else
		return stream;
}


/**
 * is_a
 */
bool DPContainer::is_a (const std::string &t)
{
	//std::cout << "my type = " << type.back() << ", compare to " << t << "\n";

	// Start with the most derived type
	for (auto it = type.rbegin(); it != type.rend(); it++) {
		if ((*it) == t) {
			return true;
		}
	}

	return false;
}

/**
 * declare
 */
void DPContainer::declare (const char *n, const char *colour)
{
	type.push_back (n);
	name = n;
	if (colour) {
		dot_colour = colour;
	}
}


/**
 * ref
 */
void DPContainer::ref (void)
{
	ref_count++;
}

/**
 * unref
 */
void DPContainer::unref (void)
{
	ref_count--;
	if (ref_count == 0)
		delete this;
}

