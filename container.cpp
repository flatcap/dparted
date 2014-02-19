/* Copyright (c) 2014 Richard Russon (FlatCap)
 *
 * This file is part of DParted.
 *
 * DParted is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DParted is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DParted.  If not, see <http://www.gnu.org/licenses/>.
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

std::vector<Action> cont_actions = {
	{ "Create/Filesystem",         true },
	{ "Create/Partition",          true },
	{ "Create/Table",              true },
	{ "Create/Lvm Volume",         true },
	{ "Create/Subvolume",          true },
	{ "Create/Snapshot",           true },
	{ "Create/Luks",               true },
	{ "Delete/Filesystem",         true },
	{ "Delete/Partition",          true },
	{ "Delete/Table",              true },
	{ "Delete/Lvm Volume",         true },
	{ "Delete/Subvolume",          true },
	{ "Delete/Snapshot",           true },
	{ "Delete/Luks",               true },
	{ "Format as/Clear",           true },
	{ "Format as/Wipe",            true },
	{ "Format as/Scrub",           true },
	{ "Format as/Ext2",            true },
	{ "Format as/Ext3",            true },
	{ "Convert to/Btrfs",          true },
	{ "Convert to/Partition Type", true },
	{ "Convert to/Table",          true },
	{ "Edit/Label",                true },
	{ "Edit/Uuid",                 true },
	{ "Edit/Flags",                true },
	{ "Edit/Parameters",           true },
	{ "Filesystem/Check",          true },
	{ "Filesystem/Defrag",         true },
	{ "Filesystem/Rebalance",      true },
	{ "Filesystem/Mount",          true },
	{ "Filesystem/Umount",         true },
	{ "Filesystem/Usage",          true },
	{ "Volume Group/Extend",       true },
	{ "Volume Group/Reduce",       true },
	{ "Volume Group/Split",        true },
	{ "Volume Group/Merge",        true },
	{ "Redundancy/Add Stripe...",     true },
	{ "Redundancy/Remove Stripe...",  true },
	{ "Redundancy/Add Mirror...",     true },
	{ "Redundancy/Remove Mirror...",  true },
	{ "Redundancy/Break Mirror...",   true }
};


Container::Container (void)
{
	const char* me = "Container";

	sub_type (me);

	declare_prop (me, "name",          name,          "desc of name");
	declare_prop (me, "uuid",          uuid,          "desc of uuid");
	declare_prop (me, "device",        device,        "desc of device");
	declare_prop (me, "parent_offset", parent_offset, "desc of parent_offset");
	declare_prop (me, "block_size",    block_size,    "desc of block_size");
	declare_prop (me, "bytes_size",    bytes_size,    "desc of bytes_size");
	declare_prop (me, "bytes_used",    bytes_used,    "desc of bytes_used");
}

Container::~Container()
{
	if (fd >= 0) {
		close (fd);
		fd = -1;
	}
}

ContainerPtr
Container::create (void)
{
	ContainerPtr c (new Container());
	c->weak = c;

	return c;
}


bool
Container::visit_children (Visitor& v)
{
	ContainerPtr c = get_smart();
	if (!v.visit_enter(c))
		return false;

	for (auto c : children) {
		if (!c->accept(v))
			return false;
	}

	if (!v.visit_leave())
		return false;

	return true;
}

bool
Container::accept (Visitor& v)
{
	ContainerPtr c = get_smart();
	if (!v.visit(c))
		return false;
	return visit_children(v);
}


void
Container::add_child (ContainerPtr& child)
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

	child->parent = get_smart();
}

void
Container::just_add_child (ContainerPtr& child)
{
	if (child)
		children.push_back (child);
	//log_debug ("just: %s (%s) -- %s\n", this->name.c_str(), child->name.c_str(), child->uuid.c_str());
}

void
Container::delete_child (ContainerPtr& child)
{
	for (auto it = children.begin(); it != children.end(); it++) {
		if (*it == child) {
			children.erase (it);
			break;
		}
	}
}

void
Container::move_child (ContainerPtr& UNUSED(child), long UNUSED(offset), long UNUSED(size))
{
}


int
Container::get_fd (void)
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

long
Container::get_block_size (void)
{
	//XXX need to get the max of all parents
	if (block_size > 0)
		return block_size;

	ContainerPtr p = parent.lock();
	if (p)
		return p->get_block_size();

	return -1;
}

std::string
Container::get_device_name (void)
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

long
Container::get_parent_offset (void)
{
	return parent_offset;
}

unsigned int
Container::get_device_space (std::map<long, long>& spaces)
{
	spaces.clear();

	//spaces[offset] = size
	//spaces[0] = 123;

	return spaces.size();;
}


long
Container::get_size_total (void)
{
	return bytes_size;
}

long
Container::get_size_used (void)
{
	return bytes_used;
}

long
Container::get_size_free (void)
{
	return bytes_size - bytes_used;
}


ContainerPtr
Container::find_device (const std::string& dev)
{
	ContainerPtr match;

	// am *I* the device?
	//log_debug ("Me? %s %s\n", device.c_str(), dev.c_str());
	if (dev == device) {
		match = get_smart();
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

ContainerPtr
Container::find_name (const std::string& search)
{
	if (name == search) {
		ContainerPtr c = get_smart();
		return c;
	}

	ContainerPtr item;

	for (auto i : children) {
		if ((item = i->find_name (search)))
			break;
	}

	return item;
}

void
Container::find_type (const std::string& type, std::vector<ContainerPtr>& results)
{
	if (is_a (type)) {
		ContainerPtr c = get_smart();
		results.push_back(c);
	}

	for (auto i : children) {
		i->find_type (type, results);
	}
}

ContainerPtr
Container::find_uuid (const std::string& search)
{
#if 0
	if (uuid.empty())
		std::cout << "name = " << name << std::endl;
	else
		std::cout << "uuid = " << uuid << std::endl;
#endif

	if (uuid == search) {
		ContainerPtr c = get_smart();
		return c;
	}

	ContainerPtr item;

	for (auto i : children) {
		if ((item = i->find_uuid (search)))
			break;
	}

	return item;
}

ContainerPtr
Container::find (const std::string& search)
{
	if (uuid == search) {
		ContainerPtr c = get_smart();
		return c;
	}

	if (name == search) {
		ContainerPtr c = get_smart();
		return c;
	}

	size_t pos = search.find ("(0)");
	if (pos == (search.length() - 3)) {
		std::string search2 = search.substr (0, pos);
		if (name == search2) {
			ContainerPtr c = get_smart();
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


unsigned char*
Container::get_buffer (long offset, long size)
{
	//XXX validate offset and size against device size
	const int PAGE_SIZE = 4096;	//XXX from kernel, use block size if bigger?

	//log_info ("object: %s (%s), device: %s, fd: %d, GET: offset: %ld, size: %s\n", name.c_str(), uuid.c_str(), device.c_str(), fd, offset, get_size (size).c_str());

	//if (mmaps.size() > 0) log_info ("%ld mmaps\n", mmaps.size());
	for (auto m : mmaps) {
		long o;
		long s;
		void* p;

		std::tie (o, s, p) = *m;
		//log_info ("mmap: %ld, %ld, %p (%ld)\n", o, s, p, m.use_count());

		//XXX allow subset: offset >= 0 and size < (s-(offset-o)?
		if ((offset == o) && (size <= s)) {
			//log_info ("mmap match: ask (%ld,%s), found (%ld,%s)\n", offset, get_size (size).c_str(), o, get_size(s).c_str());
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

	int adjust = (offset % PAGE_SIZE);
#if 0
	printf ("offset = %ld\n", offset);
	printf ("size   = %ld\n", size);
	printf ("adjust = %d\n", adjust);
	printf ("\n");
#endif
	if (adjust != 0) {
		offset -= adjust;
		size   += adjust;
	}

	size = std::max ((long) 4194304, size);	// 4 MiB

	//offset += parent_offset;
	buf = mmap (NULL, size, PROT_READ, MAP_SHARED, newfd, offset);
	if (buf == MAP_FAILED) {
		log_error ("mmap: alloc failed\n");	//XXX perror
		close (newfd);				//XXX may not be ours to close
		return nullptr;
	}
	//log_debug ("mmap created: %p, device %s, offset %ld, size %s\n", buf, device.c_str(), offset, get_size (size).c_str());

	insert (offset, size, buf);

	return ((unsigned char*) buf) + adjust;
}

void
Container::close_buffer (unsigned char* buffer, long size)
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
						<< "(" << get_size (c->bytes_size)    << "), "
		<< " U:" //<< c->bytes_used
						<< "(" << get_size (c->bytes_used)    << "), "
		<< " F:" //<<   bytes_free
						<< "(" << get_size (   bytes_free)    << "), "
		<< " P:" //<< c->parent_offset
						<< "(" << get_size (c->parent_offset) << "), "
#endif
		<< " rc: " << c.use_count()
		;

	return stream;
}


bool
Container::is_a (const std::string& t)
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

void
Container::sub_type (const char* n)
{
	name = n;
	type.push_back (name);
}


std::vector<ContainerPtr>&
Container::get_children (void)
{
	//log_info ("container::get_children\n");
	if (children.empty() && whole)
		return whole->get_children();	//XXX ARGH! don't want derived class in here

	return children;
}


void deleter (Mmap* m)
{
	long  size = 0;
	void* ptr = nullptr;

	std::tie (std::ignore, size, ptr) = *m;

	//std::cout << "mmap deleter: " << ptr << std::endl;
	munmap (ptr, size);

	delete m;
}

void
Container::insert (long offset, long size, void* ptr)
{
	mmaps.insert (MmapPtr (new Mmap (offset, size, ptr), deleter));
}


std::string
Container::get_path (void)
{
	std::string path;

	for (auto n : type) {
		if (!path.empty())
			path += '.';
		path += n;
	}

	return path;
}

std::string
Container::get_property (const std::string& propname)
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


std::vector<std::string>
Container::get_prop_names (void)
{
	std::vector<std::string> names;
	for (auto p : props) {
		names.push_back (p.second->name);
	}

	return names;
}

PPtr
Container::get_prop (const std::string& name)
{
	//std::cout << "get_prop: " << props.count (name) << std::endl;
	//XXX check exists, otherwise throw
	return props[name];
}

std::vector<PPtr>
Container::get_all_props (void)
{
	std::vector<PPtr> vv;

	for (auto p : props) {
		//XXX what's the magic C++11 way of doing this?
		vv.push_back (p.second);
	}

	return vv;
}


ContainerPtr
Container::get_smart (void)
{
	if (weak.expired()) {
		std::cout << "SMART\n";
		//XXX who created us?
		ContainerPtr c (this);
		std::cout << c << std::endl;
		weak = c;
	}
	return weak.lock();
}


std::vector<Action>
Container::get_actions (void)
{
	// LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.container", true },
	};

	return actions;
}

bool
Container::perform_action (Action action)
{
	if (action.name == "dummy.container") {
		std::cout << "Container perform: " << action.name << std::endl;
		return true;
	} else {
		std::cout << "Unknown action: " << action.name << std::endl;
		return false;
	}
}


