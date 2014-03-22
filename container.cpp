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
#include "action.h"
#include "log.h"
#include "log_trace.h"
#include "property.h"
#include "utils.h"
#include "visitor.h"
#include "whole.h"

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
	// Save a bit of space
	const char* me = "Container";
	const int   d  = (int) BaseProperty::Flags::Dot;
	const int   s  = (int) BaseProperty::Flags::Size;
	const int   p  = (int) BaseProperty::Flags::Percent;
	//const int   h  = (int) BaseProperty::Flags::Hide;

	sub_type (me);

	PPtr bs = declare_prop_fn (me, "bytes_size",    (get_uint64_t) ([&](){ return bytes_size;    }), "Size",          d|s);

	declare_prop_var_extra (me, "bytes_used", bytes_used, "Used", d|s|p, bs);

	declare_prop_fn_extra (me, "bytes_free", (get_uint64_t) std::bind(&Container::get_bytes_free, this), "desc of bytes_free", s|p, bs);

	// declare_prop_fn (me, "bytes_used",    (get_uint64_t) ([&](){ return bytes_used;    }), "Used",          d|s);
	// declare_prop_fn (me, "bytes_free",         (get_int64_t)  std::bind(&Container::get_bytes_free,         this), "desc of bytes_free",         s);

	declare_prop_fn (me, "block_size",    (get_uint64_t) ([&](){ return block_size;    }), "Block Size",    d|s);
	declare_prop_fn (me, "device",        (get_string_t) ([&](){ return device;        }), "Device",        d);
	declare_prop_fn (me, "device_major",  (get_uint64_t) ([&](){ return device_major;  }), "Major",         0);
	declare_prop_fn (me, "device_minor",  (get_uint64_t) ([&](){ return device_minor;  }), "Minor",         0);
	declare_prop_fn (me, "name",          (get_string_t) ([&](){ return name;          }), "Name",          0);
	declare_prop_fn (me, "parent_offset", (get_uint64_t) ([&](){ return parent_offset; }), "Parent Offset", d);
	declare_prop_fn (me, "uuid",          (get_string_t) ([&](){ return uuid;          }), "UUID",          0);

	//declare_prop (me, "bytes_free", bytes_free, bytes_size, "bytes free", d|s);
	//declare_prop (me, "bytes_used", bytes_used, bytes_size, "bytes used", d|s);

	declare_prop_fn (me, "device_major_minor", (get_string_t) std::bind(&Container::get_device_major_minor, this), "desc of device_major_minor", d);
	declare_prop_fn (me, "device_short",       (get_string_t) std::bind(&Container::get_device_short,       this), "desc of device_short",       d);
	declare_prop_fn (me, "name_default",       (get_string_t) std::bind(&Container::get_name_default,       this), "desc of name default",       d);
	declare_prop_fn (me, "path",               (get_string_t) std::bind(&Container::get_path,               this), "desc of get_path",           0);
	declare_prop_fn (me, "type",               (get_string_t) std::bind(&Container::get_type,               this), "desc of type",               d);
	declare_prop_fn (me, "type_long",          (get_string_t) std::bind(&Container::get_type_long,          this), "desc of type long",          0);
	declare_prop_fn (me, "uuid_short",         (get_string_t) std::bind(&Container::get_uuid_short,         this), "desc of uuid_short",         d);

	// declare_prop_fn (me, "block_size",    (get_uint64_t) ([&](){ return block_size;    }), "Block Size",    d|s);
	// declare_prop_fn (me, "bytes_free",         (get_int64_t)  std::bind(&Container::get_bytes_free,         this), "desc of bytes_free",         s);
	// declare_prop_fn (me, "bytes_size",    (get_uint64_t) ([&](){ return bytes_size;    }), "Size",          d|s);
	// declare_prop_fn (me, "bytes_used",    (get_uint64_t) ([&](){ return bytes_used;    }), "Used",          d|s);
	// declare_prop_fn (me, "chunk_size",  (get_uint64_t) ([&](){ return chunk_size;  }), "desc of chunk_size",  s);
	// declare_prop_fn (me, "data_offset", (get_uint64_t) ([&](){ return data_offset; }), "desc of data_offset", s);
	// declare_prop_fn (me, "data_size",   (get_uint64_t) ([&](){ return data_size;   }), "desc of data_size",   s);
	// declare_prop_fn (me, "metadata_size", (get_uint64_t) ([&](){ return metadata_size; }), "desc of metadata_size", s);
	// declare_prop_fn (me, "offset",     (get_uint64_t) ([&](){ return offset;     }), "desc of offset",     d|s);
	// declare_prop_fn (me, "sizelimit",  (get_uint64_t) ([&](){ return sizelimit;  }), "desc of sizelimit",  d|s);

	// Absolute Offset  bytes      u64 size        num/scale  absolute_offset_bytes      123456789

	// Absolute Offset  percentage u8  percentage  num %      absolute_offset_percentage 45
	// Absolute Size    percentage u8  percentage  num %      absolute_size_percentage   23         %age of container size
	// Parent Offset    percentage u8  percentage  num %      parent_offset_percentage   75

	// Free Space       percentage u8  percentage  num %      free_space_percentage      35
	// Size             percentage u8  percentage  num %      size_percentage            27         %age of parent's size
	// Used             percentage u8  percentage  num %      used_percentage            88

#ifdef DEBUG
	declare_prop_fn (me, "mem_addr",  (get_string_t) std::bind(&Container::get_mem_addr,  this), "desc of mem_addr",  0);
	declare_prop_fn (me, "ref_count", (get_int64_t)  std::bind(&Container::get_ref_count, this), "desc of ref_count", 0);
#endif
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
	ContainerPtr p (new Container());
	p->weak = p;

	return p;
}


bool
Container::visit_children (Visitor& v)
{
	ContainerPtr c = get_smart();
	if (!v.visit_enter(c))
		return false;

	for (auto c : children) {
		ContainerPtr p = parent.lock();
		if (!p && ((c->is_a ("Space") || c->is_a ("Filesystem")))) //XXX tmp
			continue;

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

	children.insert (child);

	//log_debug ("child: %s (%s) -- %s\n", this->name.c_str(), child->name.c_str(), child->uuid.c_str());

	child->parent = get_smart();
}

void
Container::just_add_child (ContainerPtr& child)
{
	if (child) {
		children.insert (child);
	}
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
Container::move_child (ContainerPtr& UNUSED(child), std::uint64_t UNUSED(offset), std::uint64_t UNUSED(size))
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

std::uint64_t
Container::get_block_size (void)
{
	//XXX do I need to get the max of all parents?
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

std::uint64_t
Container::get_parent_offset (void)
{
	return parent_offset;
}

std::uint64_t
Container::get_device_space (std::map<std::uint64_t, std::uint64_t>& spaces)
{
	spaces.clear();

	//spaces[offset] = size
	//spaces[0] = 123;

	return spaces.size();;
}


std::uint64_t
Container::get_size_total (void)
{
	return bytes_size;
}

std::uint64_t
Container::get_size_used (void)
{
	return bytes_used;
}

std::uint64_t
Container::get_size_free (void)
{
	return bytes_size - bytes_used;
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


void deleter (Mmap* m)
{
	long  size = 0;
	void* ptr = nullptr;

	std::tie (size, ptr) = *m;

	//std::cout << "mmap deleter: " << ptr << std::endl;
	munmap (ptr, size);

	delete m;
}

std::uint8_t*
Container::get_buffer (std::uint64_t offset, std::uint64_t size)
{
	// range check
	if ((size < 1) || ((offset + size) > bytes_size)) {
		log_error ("%s: out of range\n", __FUNCTION__);
		return nullptr;
	}

	//log_info ("object: %s (%s), device: %s, fd: %d, GET: offset: %ld, size: %s\n", name.c_str(), uuid.c_str(), device.c_str(), fd, offset, get_size (size).c_str());

	if (device_mmap) {
		void* buf = (*device_mmap).second;
		//printf ("mmap existing: %p\n", ((std::uint8_t*) buf) + offset);
		return ((std::uint8_t*) buf) + offset;
	}

	// No device -- delegate
	if (device.empty()) {
		ContainerPtr p = parent.lock();
		if (p) {
			return p->get_buffer (offset + parent_offset, size);
		} else {
			std::cout << this << std::endl;
			log_error ("%s: no device and no parent\n", __FUNCTION__);
			return nullptr;
		}
	}

	// mmap doesn't exist, create it
	void*	buf   = nullptr;
	int	newfd = get_fd();

	if (newfd < 0) {
		log_error ("%s: can't get file descriptor\n", __FUNCTION__);
		return nullptr;
	}

	// Create an mmap for the entire device

	// const long KERNEL_PAGE_SIZE = 4096;		//XXX from kernel, use block size if bigger?
	// size = std::max (KERNEL_PAGE_SIZE, size);	//XXX test on a 512 byte loop device
	size = bytes_size;	//XXX round up to PAGE_SIZE?

	buf = mmap (NULL, size, PROT_READ, MAP_SHARED, newfd, 0);
	if (buf == MAP_FAILED) {
		log_error ("%s: alloc failed\n", __FUNCTION__);	//XXX perror
		//close (newfd);				//XXX may not be ours to close
		return nullptr;
	}
	//log_debug ("mmap created: %p, device %s, size %s\n", buf, device.c_str(), get_size (size).c_str());

	device_mmap = (MmapPtr (new Mmap (size, buf), deleter));

	//printf ("mmap new: %p\n", ((std::uint8_t*) buf) + offset);
	return ((std::uint8_t*) buf) + offset;
}

void
Container::close_buffer (std::uint8_t* buffer, std::uint64_t size)
{
	if (!buffer)
		return;

	if (munmap (buffer, size) != 0)
		perror ("munmap");

	//XXX nothing else we can do
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
		uuid = "U:" + uuid.substr (0, index);
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


std::set<ContainerPtr, Container::compare>&
Container::get_children (void)
{
	//LOG_TRACE;
	if (children.empty() && whole)
		return whole->get_children();	// Delegate

	return children;
}


std::string
Container::get_path (void)
{
	//XXX same as get_type_long, change to represent ownership:
	//	disk(sda)/partition(sda1)/filesystem(ext4)
	std::string path;

	for (auto n : type) {
		if (!path.empty())
			path += '.';
		path += n;
	}

	return path;
}

#ifdef DEBUG
std::string
Container::get_mem_addr (void)
{
	std::stringstream addr;
	ContainerPtr c = weak.lock();
	if (c) {
		addr << "0x" << (void*) c.get();
	}

	return addr.str();
}

std::int64_t
Container::get_ref_count (void)
{
	return weak.use_count();
}
#endif

std::vector<std::string>
Container::get_prop_names (void)
{
	//XXX c++ magic?
	std::vector<std::string> names;
	for (auto p : props) {
		names.push_back (p.second->name);
	}

	return names;
}

PPtr
Container::get_prop (const std::string& name)
{
	if (props.count (name)) {		// Exact match
		return props[name];
	}

	return nullptr;
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
		//XXX who created us? code error
		ContainerPtr c (this);
		std::cout << c << std::endl;
		weak = c;
	}
	return weak.lock();
}


long
Container::get_bytes_free (void)
{
	return (bytes_size - bytes_used);
}

std::string
Container::get_device_major_minor (void)
{
	//XXX should this and the other device helpers ask their parents? YES
	if ((device_major == 0) && (device_minor == 0))
		return "";

	return std::to_string (device_major) + ":" + std::to_string (device_minor);
}

std::string
Container::get_device_short (void)
{
	std::string d = get_device_name();
	size_t pos = d.find ("/dev");

	if (pos != std::string::npos) {
		d = d.substr (pos+5);
	}

	return d;
}

std::string
Container::get_name_default (void)
{
	if (name.empty())
		return "[EMPTY]";
	else
		return name;
}

std::string
Container::get_type (void)
{
	return type.back();
}

std::string
Container::get_type_long (void)
{
	std::string tl;
	for (auto i : type) {
		tl += i + ".";
	}
	tl.pop_back();

	return tl;
}

std::string
Container::get_uuid_short (void)
{
	std::string u = uuid;
	if (u.length() < 8)
		return u;

	size_t pos = uuid.find_first_of ("-:");

	if (pos != std::string::npos) {
		u = uuid.substr (0, pos);
	}

	return u;
}


