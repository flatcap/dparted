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
#include <sys/mman.h>
#include <unistd.h>

#include <iterator>
#include <cstdio>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <typeinfo>
#include <algorithm>

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
	whole (nullptr),
	parent (nullptr),
	ref_count (1),
	missing (false),
	mm_fd (-1),
	mm_buffer (nullptr),
	mm_size (0)
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

	if (mm_buffer) {
		munmap (mm_buffer, mm_size);
		mm_buffer = nullptr;
		mm_size   = 0;
	}

	if (mm_fd >= 0) {
		close (mm_fd);
		mm_fd = -1;
	}
}


/**
 * operator new
 */
void *
DPContainer::operator new (size_t size)
{
	DPContainer *c = (DPContainer*) malloc (size);

	//log_info ("new object %p\n", (void*) c);

	return object_track (c);
}

/**
 * operator delete
 */
void
DPContainer::operator delete (void *ptr)
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
void
DPContainer::add_child (DPContainer *child)
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

	//log_debug ("insert: %s (%s) -- %s\n", this->name.c_str(), child->name.c_str(), child->uuid.c_str());

	child->ref();
	child->parent = this;
}

/**
 * just_add_child
 */
void
DPContainer::just_add_child (DPContainer *child)
{
	if (child)
		children.push_back (child);
}

/**
 * delete_child
 */
void
DPContainer::delete_child (DPContainer *child)
{
}

/**
 * move_child
 */
void
DPContainer::move_child (DPContainer *child, long offset, long size)
{
}


/**
 * get_block_size
 */
long
DPContainer::get_block_size (void)
{
	//XXX need to get the max of all parents
	if (block_size > 0)
		return block_size;

	if (parent)
		return parent->get_block_size();

	return -1;
}

/**
 * get_device_name
 */
std::string
DPContainer::get_device_name (void)
{
	//log_debug ("i am %s\n", typeid (*this).name());
	if (!device.empty())
		return device;
	else if (parent)
		return parent->get_device_name();
	else
		return "UNKNOWN";
}

/**
 * get_parent_offset
 */
long
DPContainer::get_parent_offset (void)
{
	return parent_offset;
}

/**
 * get_device_space
 */
unsigned int
DPContainer::get_device_space (std::map<long, long> &spaces)
{
	spaces.clear();

	//spaces[offset] = size
	//spaces[0] = 123;

	return spaces.size();;
}


/**
 * get_size_total
 */
long
DPContainer::get_size_total (void)
{
	return bytes_size;
}

/**
 * get_size_used
 */
long
DPContainer::get_size_used (void)
{
	return bytes_used;
}

/**
 * get_size_free
 */
long
DPContainer::get_size_free (void)
{
	return bytes_size - bytes_used;
}


/**
 * find_device
 */
DPContainer *
DPContainer::find_device (const std::string &dev)
{
	DPContainer *match = nullptr;

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
 * find_name
 */
DPContainer *
DPContainer::find_name (const std::string &search)
{
	if (name == search)
		return this;

	DPContainer *item = nullptr;

	for (auto i : children) {
		if ((item = i->find_name (search)))
			break;
	}

	return item;
}

/**
 * find_type
 */
void
DPContainer::find_type (const std::string &type, std::vector<DPContainer*> &results)
{
	if (is_a (type))
		results.push_back (this);

	for (auto i : children) {
		i->find_type (type, results);
	}
}

/**
 * find_uuid
 */
DPContainer *
DPContainer::find_uuid (const std::string &search)
{
	if (uuid == search)
		return this;

	DPContainer *item = nullptr;

	for (auto i : children) {
		if ((item = i->find_uuid (search)))
			break;
	}

	return item;
}

/**
 * find
 */
DPContainer *
DPContainer::find (const std::string &search)
{
	if (uuid == search)
		return this;

	if (name == search)
		return this;

	size_t pos = search.find ("(0)");
	if (pos == (search.length() - 3)) {
		std::string search2 = search.substr (0, pos);
		if (name == search2)
			return this;
	}

	DPContainer *item = nullptr;

	for (auto i : children) {
		if ((item = i->find (search)))
			break;
	}

	return item;
}


/**
 * open_device
 */
int
DPContainer::open_device (void)
{
	// flags? ro, rw
	if (mm_fd >= 0)
		return mm_fd;

	if (device.empty()) {
		if (parent) {
			mm_fd = parent->open_device();
		}
	} else {
		mm_fd = open (device.c_str(), O_RDONLY | O_CLOEXEC); // read only, close on exec
		if (mm_fd < 0) {
			log_error ("failed to open device %s\n", device.c_str());
		}
	}

	//log_info ("OPEN %s = %p\n", device.c_str(), (void*) mm_fd);

	if (mm_fd >= 0) {
		unsigned char *buffer = nullptr;
		long size = 4194304; // 4MiB
		long offset = parent_offset;

		buffer = get_buffer (offset, size);
		if (buffer) {
			mm_buffer = buffer;
			mm_size   = size;
		}
	}

	return mm_fd;
}

/**
 * read_data
 */
int
DPContainer::read_data (long offset, long size, unsigned char *buffer)
{
	// if offset >= 0, seek there
	//long current = -1;
	long bytes = 0;

	return -1;

#if 0
	if (!buffer)
		return -1;

	if (fptr == nullptr) {
		if (parent) {
			return parent->read_data (offset + parent_offset, size, buffer);
		} else {
			return -1;
		}
	}

	if (offset >= 0) {
		current = fseek (fptr, offset, SEEK_SET);
	} else {
		current = fseek (fptr, 0, SEEK_CUR);
	}

	if (current < 0) {
		log_error ("seek to %ld failed on %s (%p)\n", offset, device.c_str(), (void*) fptr);
		perror ("seek");
		return -1;
	}

	if ((current + size) > bytes_size)
		return -1;

	bytes = fread (buffer, size, 1, fptr);
	bytes *= size;
	std::string s = get_size (current);
	//log_info ("READ: device %s (%p), offset %lld (%s), size %lld = %lld\n", device.c_str(), (void*) fptr, current, s.c_str(), size, bytes);
#endif

	return bytes;
}

/**
 * get_buffer
 */
unsigned char *
DPContainer::get_buffer (long offset, long size)
{
	//XXX validate offset and size against device size

	// Mapping exists (and is big enough)
	if (mm_buffer && (mm_size >= (offset+size))) {
		return (mm_buffer + offset);
	}

	// No device -- delegate
	if (device.empty() || (mm_fd < 0)) {
		if (parent) {
			return parent->get_buffer (offset + parent_offset, size);
		} else {
			log_error ("mmap: no device and no parent\n");
			return nullptr;
		}
	}

	// Nothing yet -- allocate a big block
	void	*buf  = nullptr;
	int	 fd   = -1;

	size = std::max ((long)4194304, size);	// 4 MiB

	fd = open (device.c_str(), O_RDONLY | O_CLOEXEC); // read only, close on exec
	if (fd < 0) {
		log_error ("failed to open device %s\n", device.c_str());
		return nullptr;
	}
	log_debug ("opened device %s (%d)\n", device.c_str(), fd);

	offset += parent_offset;
	buf = mmap (NULL, size, PROT_READ, MAP_SHARED, fd, offset);
	if (buf == MAP_FAILED) {
		log_error ("mmap: alloc failed\n");	//XXX perror
		close (fd);
		return nullptr;
	}
	log_debug ("mmap'd device %s, offset %ld, size %ld\n", device.c_str(), offset, size);

	mm_buffer = (unsigned char*) buf;
	mm_size   = size;
	mm_fd     = fd;

	return mm_buffer;
}

/**
 * close_buffer
 */
void
DPContainer::close_buffer (unsigned char *buffer, long size)
{
	if (!buffer)
		return;

	if (munmap (buffer, size) != 0)
		perror ("munmap");

	// XXX nothing else we can do
}


/**
 * operator<<
 */
std::ostream &
operator<< (std::ostream &stream, const DPContainer &c)
{
#if 0
	if (c.type.back() == "filesystem")
		return stream;
#endif

	//long bytes_free = c.bytes_size - c.bytes_used;

	std::string uuid = c.uuid;

	if (uuid.size() > 8) {
		size_t index = uuid.find_first_of (":-. ");
		uuid = "U:" + uuid.substr (0, index);//RAR + "...";
	}

	stream
		<< "[" << c.type.back() << "]:"
		<< c.name << "(" << uuid << "), "
#if 0
		//<< c.device << "(" << c.fptr << "),"
		<< " S:" //<< c.bytes_size
						<< "(" << get_size(c.bytes_size)    << "), "
		<< " U:" //<< c.bytes_used
						<< "(" << get_size(c.bytes_used)    << "), "
		<< " F:" //<<   bytes_free
						<< "(" << get_size(  bytes_free)    << "), "
		<< " P:" //<< c.parent_offset
						<< "(" << get_size(c.parent_offset) << "), "
		//<< " rc: " << c.ref_count
#endif
		;

	return stream;
}

/**
 * operator<<
 */
std::ostream &
operator<< (std::ostream &stream, const DPContainer *c)
{
	if (c)
		return operator<< (stream, *c);
	else
		return stream;
}


/**
 * is_a
 */
bool
DPContainer::is_a (const std::string &t)
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
void
DPContainer::declare (const char *n)
{
	type.push_back (n);
	name = n;
}


/**
 * ref
 */
void
DPContainer::ref (void)
{
	ref_count++;
}

/**
 * unref
 */
void
DPContainer::unref (void)
{
	ref_count--;
	if (ref_count == 0) {
		//delete this;
	}
}


/**
 * dump_objects
 */
void
DPContainer::dump_objects (int indent)
{
	printf ("%*s", 8*indent, "");
	if (name == "dummy") {
		indent--;
	} else {
		std::cout << this << std::endl;
	}

	for (auto c : children) {
		c->dump_objects (indent+1);
	}
}

