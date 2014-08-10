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

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sstream>

#include "container.h"
#include "loop.h"
#include "action.h"
#include "app.h"
#include "stringnum.h"
#include "utils.h"
#include "visitor.h"
#include "log.h"

Loop::Loop (void)
{
	LOG_CTOR;
	// Save a bit of space
	const char* me = "Loop";
	const int   d  = (int) BaseProperty::Flags::Dot;
	const int   s  = (int) BaseProperty::Flags::Size;
	// const int   h  = (int) BaseProperty::Flags::Hide;

	sub_type (me);

	declare_prop_var (me, "autoclear",  autoclear,  "desc of autoclear",  0);
	declare_prop_var (me, "deleted",    deleted,    "desc of deleted",    0);
	declare_prop_var (me, "file_inode", file_inode, "desc of file_inode", d);
	declare_prop_var (me, "file_major", file_major, "desc of file_major", 0);
	declare_prop_var (me, "file_minor", file_minor, "desc of file_minor", 0);
	declare_prop_var (me, "file_name",  file_name,  "desc of file_name",  0);
	declare_prop_var (me, "offset",     offset,     "desc of offset",     d|s);
	declare_prop_var (me, "partscan",   partscan,   "desc of partscan",   0);
	declare_prop_var (me, "read_only",  read_only,  "desc of read_only",  0);
	declare_prop_var (me, "sizelimit",  sizelimit,  "desc of sizelimit",  d|s);

	declare_prop_fn (me, "file_major_minor", (get_string_t) std::bind(&Loop::get_file_major_minor, this), "desc of file_major_minor", d);
	declare_prop_fn (me, "file_name_short",  (get_string_t) std::bind(&Loop::get_file_name_short,  this), "desc of file_name_short",  d);
	declare_prop_fn (me, "flags",            (get_string_t) std::bind(&Loop::get_flags,            this), "desc of flags",            d);
}

Loop::Loop (const Loop& c) :
	Block(c)
{
	Loop();
	LOG_CTOR;
	autoclear  = c.autoclear;
	deleted    = c.deleted;
	file_inode = c.file_inode;
	file_major = c.file_major;
	file_minor = c.file_minor;
	file_name  = c.file_name;
	offset     = c.offset;
	partscan   = c.partscan;
	read_only  = c.read_only;
	sizelimit  = c.sizelimit;
}

Loop::Loop (Loop&& c)
{
	LOG_CTOR;
	swap(c);
}

Loop::~Loop()
{
	LOG_DTOR;
}

LoopPtr
Loop::create (const std::string& losetup)
{
	std::vector<std::string> parts;

	explode (" :", losetup, parts, 12);

#if 0
	log_info ("parts: (%ld)", parts.size());
	for (auto& i : parts) {
		log_debug ("\t%s", i.c_str());
	}
#endif

	//XXX validate all input, else throw()

	LoopPtr l (new Loop());
	l->self = l;

	l->device     = parts[0];
	l->file_name  = parts[11];

	l->name = l->device;
	std::size_t index = l->name.find_last_of ('/');
	if (index != std::string::npos) {
		l->name = l->name.substr (index+1);
	}

	l->autoclear    = StringNum (parts[ 1]);
	l->file_inode   = StringNum (parts[ 2]);
	l->file_major   = StringNum (parts[ 3]);
	l->file_minor   = StringNum (parts[ 4]);
	l->device_major = StringNum (parts[ 5]);
	l->device_minor = StringNum (parts[ 6]);
	l->offset       = StringNum (parts[ 7]);
	l->partscan     = StringNum (parts[ 8]);
	l->read_only    = StringNum (parts[ 9]);
	l->sizelimit    = StringNum (parts[10]);

	std::size_t len = l->file_name.size();
	if ((len > 10) && (l->file_name.substr (len-10) == " (deleted)")) {
		l->file_name.erase (len-10);
		l->deleted = true;
		log_info ("%s is deleted", l->device.c_str());
	}

	l->block_size   = 512;	//XXX kernel lower limit, but fs block size is likely to be bigger

	std::stringstream ss;
	ss << "[" << l->device_major << ":" << l->device_minor << "]";
	l->uuid = ss.str();

	return l;
}


Loop&
Loop::operator= (const Loop& c)
{
	autoclear  = c.autoclear;
	deleted    = c.deleted;
	file_inode = c.file_inode;
	file_major = c.file_major;
	file_minor = c.file_minor;
	file_name  = c.file_name;
	offset     = c.offset;
	partscan   = c.partscan;
	read_only  = c.read_only;
	sizelimit  = c.sizelimit;

	return *this;
}

Loop&
Loop::operator= (Loop&& c)
{
	swap(c);
	return *this;
}


void
Loop::swap (Loop& c)
{
	std::swap (autoclear,  c.autoclear);
	std::swap (deleted,    c.deleted);
	std::swap (file_inode, c.file_inode);
	std::swap (file_major, c.file_major);
	std::swap (file_minor, c.file_minor);
	std::swap (file_name,  c.file_name);
	std::swap (offset,     c.offset);
	std::swap (partscan,   c.partscan);
	std::swap (read_only,  c.read_only);
	std::swap (sizelimit,  c.sizelimit);
}

void
swap (Loop& lhs, Loop& rhs)
{
	lhs.swap (rhs);
}


Loop*
Loop::clone (void)
{
	LOG_TRACE;
	return new Loop (*this);
}


bool
Loop::accept (Visitor& v)
{
	LoopPtr l = std::dynamic_pointer_cast<Loop> (get_smart());
	if (!v.visit(l))
		return false;

	return visit_children(v);
}


std::vector<Action>
Loop::get_actions (void)
{
	LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.loop", true },
	};

	std::vector<Action> parent_actions = Block::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
Loop::perform_action (Action action)
{
	if (action.name == "dummy.loop") {
		log_debug ("Loop perform: %s", action.name.c_str());
		return true;
	} else {
		return Block::perform_action (action);
	}
}


bool
Loop::losetup (std::vector <std::string>& output, std::string device)
{
	/* Alternative source of info:
	 *	/sys/devices/virtual/block/loop48/
	 *		dev
	 *		ro
	 *		size
	 *		loop/autoclear
	 *		loop/backing_file
	 *		loop/offset
	 *		loop/partscan
	 *		loop/sizelimit
	 * Need to stat backing file for inode, kernel major, kernel minor
	 */

	/* Limitations of using "losetup" output:
	 *	Filename begins, or ends, with whitespace
	 *	Filename ends " (deleted)"
	 */
	/* Example output:
	 *	/dev/loop17 0 47 8:3 7:272 0 1 0 0 /mnt/space/test/test_24.img
	 *	/dev/loop18 0 48 8:3 7:288 0 1 0 0 /mnt/space/test/test_30.img (deleted)
	 */

	std::string command = "losetup --noheadings --output name,autoclear,back-ino,back-maj:min,maj:min,offset,partscan,ro,sizelimit,back-file";

	if (!device.empty()) {
		command += " " + device;
	}

	output.clear();
	//XXX distinguish between zero loop devices and an error
	int retval = execute_command_out (command, output);
	/* retval:
	 *	0 matches
	 *	0 no matches
	 *	1 device doesn't exist
	 *	1 invalid arguments
	 */
	if (output.empty())
		return false;

	return (retval == 0);
}

void
Loop::discover (ContainerPtr& parent)
{
	return_if_fail (parent);
	LOG_TRACE;

	std::vector <std::string> output;

	if (!losetup (output))
		return;

	std::vector<LoopPtr> loops;
	for (auto& line : output) {
		LoopPtr l = create (line);

		l->get_fd();

		off_t size;
		size = lseek (l->fd, 0, SEEK_END);	//XXX move to container::find_size or utils (or block::)
		l->bytes_size = size;

		loops.push_back(l);
	}

	// The transaction manages the write lock
	ContainerPtr new_parent = Container::start_transaction (parent, "Loop: discover devices");
	if (!new_parent)
		return;

	for (auto l : loops) {
		new_parent->add_child (l, true);
	}

	Container::commit_transaction();
}

bool
Loop::identify (ContainerPtr& parent, const std::string& name, int fd, struct stat& UNUSED(st))
{
	return_val_if_fail (parent, false);
	return_val_if_fail (!name.empty(), false);
	return_val_if_fail (fd>=0, false);
	LOG_TRACE;

	off_t size;

	std::vector <std::string> output;

	losetup (output, name);		//XXX retval, exactly one reply

	LoopPtr l = create (output[0]);

	size = lseek (fd, 0, SEEK_END);	//XXX isn't the stat buf enough?

	l->device        = name;
	l->parent_offset = 0;
	l->bytes_size    = size;
	l->bytes_used    = 0;

	std::size_t i = l->device.find_last_of ('/');
	if (i == std::string::npos) {
		l->name = l->device;
	} else {
		l->name = l->device.substr (i+1);
	}

	l->block_size = 512;	//XXX granularity, or blocksize of backing-file fs/disk?

	std::stringstream ss;
	ss << "[" << l->device_major << ":" << l->device_minor << "]";
	l->uuid = ss.str();

	//RAR std::string desc = "Identified loopback device: " + l->get_device_short();
	parent->add_child (l, true);
	return true;
}


std::string
Loop::get_file_major_minor (void)
{
	if (file_major == 0)
		return "";

	return std::to_string (file_major) + ":" + std::to_string (file_minor);
}

std::string
Loop::get_file_name_short (void)
{
	std::size_t pos = file_name.find_last_of ('/');
	if (pos == std::string::npos)
		return file_name;

	return file_name.substr (pos+1);
}

std::string
Loop::get_flags (void)
{
	std::string flags;

	if (autoclear) flags += "autoclear, ";
	if (deleted)   flags += "deleted, ";
	if (partscan)  flags += "partscan, ";
	if (read_only) flags += "read_only, ";

	if (flags.length() > 2) {
		flags.pop_back();
		flags.pop_back();
	}

	if (flags.empty())
		flags = "n/a";

	return flags;
}


