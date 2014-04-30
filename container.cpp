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
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <iterator>
#include <mutex>
#include <set>
#include <sstream>
#include <string>
#include <typeinfo>

#include "app.h"
#include "container.h"
#include "action.h"
#include "log.h"
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
	log_ctor ("ctor Container");
	// Save a bit of space
	const char* me = "Container";
	const int   d  = (int) BaseProperty::Flags::Dot;
	const int   s  = (int) BaseProperty::Flags::Size;
	const int   p  = (int) BaseProperty::Flags::Percent;
	const int   h  = (int) BaseProperty::Flags::Hide;

	sub_type (me);

	PPtr tls = declare_prop_fn  (me, "top_level_size", (get_uint64_t) std::bind(&Container::get_top_level_size, this),  "desc of tls", s|h);
	PPtr ps  = declare_prop_fn  (me, "parent_size",    (get_uint64_t) std::bind(&Container::get_parent_size,    this),  "desc of ps",  s|h);
	PPtr bs  = declare_prop_var (me, "bytes_size",     bytes_size,                                                      "Size",        d|s|p, ps);

	declare_prop_var (me, "absolute_size", bytes_size,    "desc of abs size",  s|p,   tls);
	declare_prop_var (me, "block_size",    block_size,    "Block Size",        d|s);
	declare_prop_var (me, "bytes_used",    bytes_used,    "Used",              d|s|p, bs);
	declare_prop_var (me, "device",        device,        "Device",            d);
	declare_prop_var (me, "device_major",  device_major,  "Major",             0);
	declare_prop_var (me, "device_minor",  device_minor,  "Minor",             0);
	declare_prop_var (me, "name",          name,          "Name",              0);
	declare_prop_var (me, "parent_offset", parent_offset, "Parent Offset",     d|s|p, ps);
	declare_prop_var (me, "uuid",          uuid,          "UUID",              0);
	declare_prop_var (me, "file_desc",     fd,            "desc of ref_count", 0);

	declare_prop_fn (me, "device_inherit",             (get_string_t) std::bind(&Container::get_device_inherit,             this), "desc of ",                   0);
	declare_prop_fn (me, "device_major_inherit",       (get_uint64_t) std::bind(&Container::get_device_major_inherit,       this), "desc of ",                   0);
	declare_prop_fn (me, "device_minor_inherit",       (get_uint64_t) std::bind(&Container::get_device_minor_inherit,       this), "desc of ",                   0);
	declare_prop_fn (me, "device_major_minor",         (get_string_t) std::bind(&Container::get_device_major_minor,         this), "desc of device_major_minor", d);
	declare_prop_fn (me, "device_short",               (get_string_t) std::bind(&Container::get_device_short,               this), "desc of device_short",       d);
	declare_prop_fn (me, "device_major_minor_inherit", (get_string_t) std::bind(&Container::get_device_major_minor_inherit, this), "desc of device_major_minor", 0);
	declare_prop_fn (me, "device_short_inherit",       (get_string_t) std::bind(&Container::get_device_short_inherit,       this), "desc of device_short",       0);
	declare_prop_fn (me, "name_default",               (get_string_t) std::bind(&Container::get_name_default,               this), "desc of name default",       d);
	declare_prop_fn (me, "path_name",                  (get_string_t) std::bind(&Container::get_path_name,                  this), "desc of get_path_name",      0);
	declare_prop_fn (me, "path_type",                  (get_string_t) std::bind(&Container::get_path_type,                  this), "desc of get_path_type",      0);
	declare_prop_fn (me, "type",                       (get_string_t) std::bind(&Container::get_type,                       this), "desc of type",               d);
	declare_prop_fn (me, "type_long",                  (get_string_t) std::bind(&Container::get_type_long,                  this), "desc of type long",          0);
	declare_prop_fn (me, "uuid_short",                 (get_string_t) std::bind(&Container::get_uuid_short,                 this), "desc of uuid_short",         d);
	declare_prop_fn (me, "bytes_free",                 (get_uint64_t) std::bind(&Container::get_bytes_free,                 this), "desc of bytes_free",         s|p, bs);
	declare_prop_fn (me, "absolute_offset",            (get_uint64_t) std::bind(&Container::get_absolute_offset,            this), "desc of abs off",            s|p, tls);
	declare_prop_fn (me, "file_desc_inherit",          (get_int64_t)  std::bind(&Container::get_file_desc_inherit,          this), "desc of file_desc_inherit",  0);
	declare_prop_fn (me, "object_addr",                (get_string_t) std::bind(&Container::get_object_addr,                this), "desc of object_addr",        0);
	declare_prop_fn (me, "ref_count",                  (get_int64_t)  std::bind(&Container::get_ref_count,                  this), "desc of ref_count",          0);
	declare_prop_fn (me, "mmap_addr",                  (get_string_t) std::bind(&Container::get_mmap_addr,                  this), "desc of ref_count",          0);
	declare_prop_fn (me, "mmap_size",                  (get_uint64_t) std::bind(&Container::get_mmap_size,                  this), "desc of ref_count",          0);

	more_props.resize (10);		//XXX this will break the properties if it gets reallocated
}

Container::~Container()
{
	if (fd >= 0) {
		close (fd);
		log_file ("file close: %d", fd);
		fd = -1;
	}
	log_dtor ("dtor Container");
}

ContainerPtr
Container::create (void)
{
	ContainerPtr p (new Container());
	p->self = p;

	return p;
}


bool
Container::visit_children (Visitor& v)
{
	ContainerPtr cont = get_smart();
	if (!v.visit_enter(cont))
		return false;

	for (auto child : children) {
		if (!child->accept(v))
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
	LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.container", true },
	};

	return actions;
}

bool
Container::perform_action (Action action)
{
	if (action.name == "dummy.container") {
		log_debug ("Container perform: %s", action.name.c_str());
		return true;
	} else {
		log_debug ("Unknown action: %s", action.name.c_str());
		return false;
	}
}


void
Container::add_child (ContainerPtr& child, bool probe)
{
	return_if_fail (child);
	LOG_TRACE;

	++seqnum;
	children.insert (child);

	if (probe)
		main_app->queue_add_probe (child);

	if (bytes_size == 0)	// We are a dummy device
		return;

	child->parent = get_smart();

	/* Check:
	 *	available space
	 *	alignment type
	 *	size (restrictions)
	 *	valid type within this parent
	 */

	bytes_used += child->bytes_size;

	log_debug ("child: %s (%s) -- %s", this->name.c_str(), child->name.c_str(), child->uuid.c_str());

	ContainerPtr parent;
	ContainerPtr p = get_parent();
	while (p) {
		parent = p;
		p = p->get_parent();
	}

	const char* name = "";
	if (parent) {
		name = parent->name.c_str();
	}
	log_debug ("TOPLEVEL = %p (%s)", (void*) parent.get(), name);

#if 0
	log_debug ("%12lu %12lu %12lu %12lu",
		child->parent_offset,
		child->bytes_size,
		child->bytes_used,
		child->get_bytes_free());
#endif
}

void
Container::delete_child (ContainerPtr& child)
{
	for (auto it = children.begin(); it != children.end(); ++it) {
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

		log_error ("no device to open");
		return -1;
	}

	int newfd = open (device.c_str(), O_RDONLY | O_CLOEXEC); // read only, close on exec
	if (newfd < 0) {
		log_error ("failed to open device %s", device.c_str());
		return -1;
	}
	log_file ("file open: %d, '%s'", newfd, device.c_str());

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
	log_debug ("i am %s", demangle (typeid (*this).name()).c_str());
	if (!device.empty())
		return device;

	ContainerPtr p = parent.lock();
	if (p) {
		return p->get_device_name();
	} else {
		return "UNKNOWN";
	}
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

	return spaces.size();
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

	std::size_t pos = search.find ("(0)");
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
	return_if_fail (m);

	std::uint64_t size = 0;
	void* ptr = nullptr;

	std::tie (size, ptr) = *m;

	log_file ("mmap deleter: %p", ptr);
	munmap (ptr, size);

	delete m;
}

std::uint8_t*
Container::get_buffer (std::uint64_t offset, std::uint64_t size)
{
	// range check
	if ((size < 1) || ((offset + size) > bytes_size)) {
		log_error ("out of range");
		return nullptr;
	}

	log_info ("object: %s (%s), device: %s, fd: %d, GET: offset: %ld, size: %s", name.c_str(), uuid.c_str(), device.c_str(), fd, offset, get_size (size).c_str());

	if (device_mmap) {
		void* buf = (*device_mmap).second;
		log_debug ("mmap existing: %p", ((std::uint8_t*) buf) + offset);
		return ((std::uint8_t*) buf) + offset;
	}

	// No device -- delegate
	if (device.empty()) {
		ContainerPtr p = parent.lock();
		if (p) {
			return p->get_buffer (offset + parent_offset, size);
		} else {
			log_debug (this->dump());
			log_error ("no device and no parent");
			return nullptr;
		}
	}

	// mmap doesn't exist, create it
	void*	buf   = nullptr;
	int	newfd = get_fd();

	if (newfd < 0) {
		log_error ("can't get file descriptor");
		return nullptr;
	}

	// Create an mmap for the entire device

	// const std::uint64_t KERNEL_PAGE_SIZE = 4096;		//XXX from kernel, use block size if bigger?
	// size = std::max (KERNEL_PAGE_SIZE, size);	//XXX test on a 512 byte loop device
	size = bytes_size;	//XXX round up to PAGE_SIZE?

	buf = mmap (NULL, size, PROT_READ, MAP_SHARED, newfd, 0);
	if (buf == MAP_FAILED) {
		log_error ("alloc failed: %s", strerror (errno));
		//close (newfd);				//XXX may not be ours to close
		return nullptr;
	}
	log_file ("mmap created: %p, device %s, size %s", buf, device.c_str(), get_size (size).c_str());

	device_mmap = (MmapPtr (new Mmap (size, buf), deleter));

	log_debug ("mmap new: %p", ((std::uint8_t*) buf) + offset);
	return ((std::uint8_t*) buf) + offset;
}

void
Container::close_buffer (std::uint8_t* buffer, std::uint64_t size)
{
	return_if_fail (buffer);

	if (munmap (buffer, size) != 0) {
		log_error ("munmap failed: %s", strerror (errno));
		// nothing else we can do
	}
}


/**
 * operator<<
 */
std::ostream&
operator<< (std::ostream& stream, const ContainerPtr& c)
{
	return_val_if_fail (c, stream);

	//std::uint64_t bytes_free = c.bytes_size - c->bytes_used;

	std::string uuid = c->uuid;

	if (uuid.size() > 8) {
		std::size_t index = uuid.find_first_of (":-. ");
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
		<< " seq: " << c->seqnum
		;

	return stream;
}


bool
Container::is_a (const std::string& t)
{
	log_debug ("my type = %s, compare to %s", type.back().c_str(), t.c_str());

	// Start with the most derived type
	for (auto it = type.rbegin(); it != type.rend(); ++it) {
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
	LOG_TRACE;
	if (children.empty() && whole)
		return whole->get_children();	// Delegate

	return children;
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
	if (props.count (name)) {		// Exact match
		return props[name];
	}

	return nullptr;
}

std::vector<PPtr>
Container::get_all_props (bool inc_hidden /*=false*/)
{
	std::vector<PPtr> vv;

	for (auto p : props) {
		if ((p.second->flags & BaseProperty::Flags::Hide) && !inc_hidden)
			continue;

		vv.push_back (p.second);
	}

	return vv;
}


PPtr
Container::add_string_prop (const std::string& owner, const std::string& name, const std::string& value)
{
	more_props.push_back (value);
	PPtr p = declare_prop_var (owner.c_str(), name.c_str(), more_props.back(), "desc", 0);
	return p;
}


ContainerPtr
Container::get_smart (void)
{
	return_val_if_fail (!self.expired(), nullptr);

	return self.lock();
}

ContainerPtr
Container::get_parent (void)
{
	return parent.lock();
}


std::uint64_t
Container::get_absolute_offset (void)
{
	std::uint64_t ao = parent_offset;

	ContainerPtr p = get_smart();
	if (!p)
		return ao;

	while ((p = p->parent.lock())) {
		ao += p->parent_offset;
	}

	return ao;
}

std::uint64_t
Container::get_bytes_free (void)
{
	return (bytes_size - bytes_used);
}

std::string
Container::get_device_inherit (void)
{
	if (!device.empty())
		return device;

	ContainerPtr p = get_smart();
	if (!p)
		return "";

	while ((p = p->parent.lock())) {
		if (!p->device.empty())
			return p->device;
	}

	return "";
}

std::uint64_t
Container::get_device_major_inherit (void)
{
	if (device_major > 0)
		return device_major;

	ContainerPtr p = get_smart();
	if (!p)
		return 0;

	while ((p = p->parent.lock())) {
		if (p->device_major > 0)
			return p->device_major;
	}

	return 0;
}

std::string
Container::get_device_major_minor (void)
{
	if (device_major > 0)
		return std::to_string (device_major) + ":" + std::to_string (device_minor);

	return "";
}

std::string
Container::get_device_major_minor_inherit (void)
{
	if (device_major > 0)
		return std::to_string (device_major) + ":" + std::to_string (device_minor);

	ContainerPtr p = get_smart();
	if (!p)
		return "";

	while ((p = p->parent.lock())) {
		if (p->device_major > 0)
			return std::to_string (p->device_major) + ":" + std::to_string (p->device_minor);
	}

	return "";
}

std::uint64_t
Container::get_device_minor_inherit (void)
{
	if (device_major > 0)			// This is not a typo
		return device_minor;

	ContainerPtr p = get_smart();
	if (!p)
		return 0;

	while ((p = p->parent.lock())) {
		if (device_major > 0)		// This is not a typo
			return device_minor;
	}

	return 0;
}

std::string
Container::get_device_short (void)
{
	return shorten_device (device);
}

std::string
Container::get_device_short_inherit (void)
{
	if (!device.empty())
		return shorten_device (device);

	ContainerPtr p = get_smart();
	if (!p)
		return "";

	while ((p = p->parent.lock())) {
		if (!p->device.empty())
			return shorten_device (p->device);
	}

	return "";
}

std::uint64_t
Container::get_file_desc_inherit (void)
{
	if (fd >= 0)
		return fd;

	ContainerPtr p = get_smart();
	if (!p)
		return -1;

	while ((p = p->parent.lock())) {
		if (p->fd >= 0)
			return p->fd;
	}

	return -1;
}

std::string
Container::get_mmap_addr (void)
{
	std::stringstream addr;

	if (device_mmap) {
		addr << (void*) device_mmap->second;
	}

	return addr.str();
}

std::uint64_t
Container::get_mmap_size (void)
{
	if (device_mmap) {
		return device_mmap->first;
	} else {
		return 0;
	}
}

std::string
Container::get_name_default (void)
{
	if (name.empty()) {
		return "[EMPTY]";
	} else {
		return name;
	}
}

std::string
Container::get_object_addr (void)
{
	std::stringstream addr;

	ContainerPtr c = self.lock();
	if (c) {
		addr << (void*) c.get();
	}

	return addr.str();
}

std::uint64_t
Container::get_parent_size (void)
{
	ContainerPtr p = parent.lock();
	if (!p)
		return 0;

	return p->bytes_size;
}

std::string
Container::get_path_name (void)
{
	std::string path = "/" + get_name_default();

	ContainerPtr p = get_smart();
	if (!p)
		return path;

	while ((p = p->parent.lock())) {
		path = "/" + p->get_name_default() + path;
	}

	return path;
}

std::string
Container::get_path_type (void)
{
	std::string path = "/" + type.back();

	ContainerPtr p = get_smart();
	if (!p)
		return path;

	while ((p = p->parent.lock())) {
		path = "/" + p->type.back() + path;
	}

	return path;
}

std::int64_t
Container::get_ref_count (void)
{
	return self.use_count();
}

std::uint64_t
Container::get_top_level_size (void)
{
	std::uint64_t tls = bytes_size;

	ContainerPtr p = get_smart();
	if (!p)
		return tls;

	while ((p = p->parent.lock())) {
		if (p->bytes_size > tls) {
			tls = p->bytes_size;
		}
	}

	return tls;
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

	std::size_t pos = uuid.find_first_of ("-:");
	if (pos != std::string::npos) {
		u = uuid.substr (0, pos);
	}

	return u;
}

std::string
Container::dump (void)
{
	std::stringstream s;
	s << this;
	return s.str();
}


