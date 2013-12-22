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
#include "whole.h"
#include "log.h"
#include "utils.h"
#include "visitor.h"
#include "log_trace.h"

/**
 * DPContainer
 */
DPContainer::DPContainer (void)
{
}

/**
 * ~DPContainer
 */
DPContainer::~DPContainer()
{
	if (fd >= 0) {
		close (fd);
		fd = -1;
	}
}

/**
 * create
 */
ContainerPtr
DPContainer::create (void)
{
	ContainerPtr c (new DPContainer());
	c->weak = c;
	c->declare ("container");

	return c;
}


/**
 * visit_children
 */
bool
DPContainer::visit_children (Visitor& v)
{
	ContainerPtr c = get_smart();
	if (!v.visit_enter (c))
		return false;

	for (auto c : children) {
		if (!c->accept (v))
			return false;
	}

	if (!v.visit_leave())
		return false;

	return true;
}

/**
 * accept
 */
bool
DPContainer::accept (Visitor& v)
{
	ContainerPtr c = get_smart();
	if (!v.visit (c))
		return false;
	return visit_children (v);
}


/**
 * add_child
 */
void
DPContainer::add_child (ContainerPtr& child)
{
	if (!child)
		return;

	/* Check:
	 *	available space
	 *	alignment type
	 *	size (restrictions)
	 *	valid type within this parent
	 */
#if 0
	if (children.size() > 0) {
		ContainerPtr last = children.back();

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

	//log_debug ("child: %s (%s) -- %s\n", this->name.c_str(), child->name.c_str(), child->uuid.c_str());

	//child->parent.reset (this);
	child->parent = get_smart();
}

/**
 * just_add_child
 */
void
DPContainer::just_add_child (ContainerPtr& child)
{
	if (child)
		children.push_back (child);
	//log_debug ("just: %s (%s) -- %s\n", this->name.c_str(), child->name.c_str(), child->uuid.c_str());
}

/**
 * delete_child
 */
void
DPContainer::delete_child (ContainerPtr& child)
{
	for (auto it = children.begin(); it != children.end(); it++) {
		if (*it == child) {
			children.erase (it);
			break;
		}
	}
}

/**
 * move_child
 */
void
DPContainer::move_child (ContainerPtr& child, long offset, long size)
{
}


/**
 * get_fd
 */
int
DPContainer::get_fd (void)
{
	if (fd >= 0)
		return fd;

	std::string device = get_device_name();
	if (device.empty()) {
		ContainerPtr p = parent.lock();
		if (p)
			return p->get_fd();

		log_error ("no device to open\n");
		return -1;
	}

	int newfd = open (device.c_str(), O_RDONLY | O_CLOEXEC); // read only, close on exec
	if (newfd < 0) {
		log_error ("failed to open device %s\n", device.c_str());
		return -1;
	}
	//log_debug ("opened device %s (%d)\n", device.c_str(), newfd);

	fd = newfd;

	return newfd;
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

	ContainerPtr p = parent.lock();
	if (p)
		return p->get_block_size();

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

	ContainerPtr p = parent.lock();
	if (p)
		return p->get_device_name();
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
DPContainer::get_device_space (std::map<long, long>& spaces)
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
ContainerPtr
DPContainer::find_device (const std::string& dev)
{
	ContainerPtr match;

	// am *I* the device?
	//log_debug ("Me? %s %s\n", device.c_str(), dev.c_str());
	if (dev == device) {
		match.reset (this);
		return match;
	}

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
ContainerPtr
DPContainer::find_name (const std::string& search)
{
	if (name == search) {
		ContainerPtr c(this);
		return c;
	}

	ContainerPtr item;

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
DPContainer::find_type (const std::string& type, std::vector<ContainerPtr>& results)
{
	if (is_a (type)) {
		ContainerPtr c(this);
		results.push_back (c);
	}

	for (auto i : children) {
		i->find_type (type, results);
	}
}

/**
 * find_uuid
 */
ContainerPtr
DPContainer::find_uuid (const std::string& search)
{
	if (uuid == search) {
		ContainerPtr c(this);
		return c;
	}

	ContainerPtr item;

	for (auto i : children) {
		if ((item = i->find_uuid (search)))
			break;
	}

	return item;
}

/**
 * find
 */
ContainerPtr
DPContainer::find (const std::string& search)
{
	if (uuid == search) {
		ContainerPtr c(this);
		return c;
	}

	if (name == search) {
		ContainerPtr c(this);
		return c;
	}

	size_t pos = search.find ("(0)");
	if (pos == (search.length() - 3)) {
		std::string search2 = search.substr (0, pos);
		if (name == search2) {
			ContainerPtr c(this);
			return c;
		}
	}

	ContainerPtr item;

	for (auto i : children) {
		if ((item = i->find (search)))
			break;
	}

	return item;
}


/**
 * get_buffer
 */
unsigned char*
DPContainer::get_buffer (long offset, long size)
{
	//XXX validate offset and size against device size

	//log_info ("object: %s (%s), device: %s, fd: %d, GET: offset: %ld, size: %s\n", name.c_str(), uuid.c_str(), device.c_str(), fd, offset, get_size(size).c_str());

	//if (mmaps.size() > 0) log_info ("%ld mmaps\n", mmaps.size());
	for (auto m : mmaps) {
		long o;
		long s;
		void* p;

		std::tie (o, s, p) = *m;
		//log_info ("mmap: %ld, %ld, %p (%ld)\n", o, s, p, m.use_count());

		if ((offset == o) && (size <= s)) {
			//log_info ("mmap match: ask (%ld,%s), found (%ld,%s)\n", offset, get_size(size).c_str(), o, get_size(s).c_str());
			return (unsigned char*) p;
		}
	}

	// No device -- delegate
	if (device.empty()) {
		ContainerPtr p = parent.lock();
		if (p) {
			return p->get_buffer (offset + parent_offset, size);
		} else {
			std::cout << this << std::endl;
			log_error ("mmap: no device and no parent\n");
			return nullptr;
		}
	}

	// mmap doesn't exist, or isn't big enough
	void*	buf   = nullptr;
	int	newfd = get_fd();

	if (newfd < 0) {
		log_error ("can't get file descriptor\n");
		return nullptr;
	}

	size = std::max ((long)4194304, size);	// 4 MiB

	//offset += parent_offset;
	buf = mmap (NULL, size, PROT_READ, MAP_SHARED, newfd, offset);
	if (buf == MAP_FAILED) {
		log_error ("mmap: alloc failed\n");	//XXX perror
		close (newfd);				//XXX may not be ours to close
		return nullptr;
	}
	//log_debug ("mmap created: %p, device %s, offset %ld, size %s\n", buf, device.c_str(), offset, get_size(size).c_str());

	insert (offset, size, buf);

	return (unsigned char*) buf;
}

/**
 * close_buffer
 */
void
DPContainer::close_buffer (unsigned char* buffer, long size)
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
std::ostream&
operator<< (std::ostream& stream, const ContainerPtr& c)
{
#if 0
	if (c.type.back() == "filesystem")
		return stream;
#endif

	//long bytes_free = c.bytes_size - c->bytes_used;

	std::string uuid = c->uuid;

	if (uuid.size() > 8) {
		size_t index = uuid.find_first_of (":-. ");
		uuid = "U:" + uuid.substr (0, index);//RAR + "...";
	}

	stream
		<< "[" << c->type.back() << "]:"
		<< c->name << "(" << uuid << "), "
		<< '"' << c->device << '"' << "(" << c->fd << "),"
#if 0
		<< " S:" //<< c->bytes_size
						<< "(" << get_size(c->bytes_size)    << "), "
		<< " U:" //<< c->bytes_used
						<< "(" << get_size(c->bytes_used)    << "), "
		<< " F:" //<<   bytes_free
						<< "(" << get_size(  bytes_free)    << "), "
		<< " P:" //<< c->parent_offset
						<< "(" << get_size(c->parent_offset) << "), "
		//<< " rc: " << c->ref_count
#endif
		;

	return stream;
}


/**
 * is_a
 */
bool
DPContainer::is_a (const std::string& t)
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
DPContainer::declare (const char* n)
{
	name = n;
	type.push_back (name);
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
		std::cout << get_smart() << std::endl;
	}

	for (auto c : children) {
		c->dump_objects (indent+1);
	}
}


/**
 * get_children
 */
std::vector<ContainerPtr>&
DPContainer::get_children (void)
{
	//log_info ("container::get_children\n");
	if (children.empty() && whole)
		return whole->get_children();	//XXX ARGH! don't want derived class in here

	return children;
}


/**
 * deleter
 */
void deleter (Mmap* m)
{
	long  size;
	void* ptr;

	std::tie (std::ignore, size, ptr) = *m;

	//std::cout << "mmap deleter: " << ptr << std::endl;
	munmap (ptr, size);

	delete m;
}

/**
 * insert
 */
void
DPContainer::insert (long offset, long size, void* ptr)
{
	mmaps.insert (MmapPtr(new Mmap (offset, size, ptr), deleter));
}


/**
 * get_path
 */
std::string
DPContainer::get_path (void)
{
	std::string path;

	for (auto n : type) {
		if (!path.empty())
			path += '.';
		path += n;
	}

	return path;
}

/**
 * get_property
 */
std::string
DPContainer::get_property (const std::string& propname)
{
	if (propname == "name") {
		return name;
	} else if (propname == "uuid") {
		return uuid;
	} else if (propname == "uuid_short") {
		std::string uuid_short = uuid;

		if ((uuid_short.size() > 8) && (uuid_short[0] != '/')) {
			uuid_short = uuid_short.substr (0, 6) + "...";
		}

		return uuid_short;
	} else if (propname == "device") {
		return get_device_name();
	} else if (propname == "device_short") {
		std::string d = get_device_name();
		size_t pos = d.find_last_of ('/');
		if (pos != std::string::npos) {
			d.erase (0, pos+1);
		}
		return d;
	} else if (propname == "parent_offset") {
		return std::to_string (parent_offset);
	} else if (propname == "block_size") {
		return std::to_string (block_size);
	} else if (propname == "bytes_size") {
		return std::to_string (bytes_size);
	} else if (propname == "bytes_size_human") {
		return get_size (bytes_size);
	} else if (propname == "bytes_used") {
		return std::to_string (bytes_used);
	} else if (propname == "bytes_used_human") {
		return get_size (bytes_used);
	}

	return propname;
}

