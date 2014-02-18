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

#include "app.h"
#include "loop.h"
#include "log_trace.h"
#include "utils.h"
#include "main.h"
#include "stringnum.h"
#include "visitor.h"

/**
 * Loop
 */
Loop::Loop (void)
{
	const char* me = "Loop";

	sub_type (me);

	declare_prop (me, "file_name",  file_name,  "desc of file_name");
	declare_prop (me, "file_inode", file_inode, "desc of file_inode");
	declare_prop (me, "file_major", file_major, "desc of file_major");
	declare_prop (me, "file_minor", file_minor, "desc of file_minor");
	declare_prop (me, "loop_major", loop_major, "desc of loop_major");
	declare_prop (me, "loop_minor", loop_minor, "desc of loop_minor");
	declare_prop (me, "offset",     offset,     "desc of offset");
	declare_prop (me, "sizelimit",  sizelimit,  "desc of sizelimit");
	declare_prop (me, "autoclear",  autoclear,  "desc of autoclear");
	declare_prop (me, "partscan",   partscan,   "desc of partscan");
	declare_prop (me, "read_only",  read_only,  "desc of read_only");
	declare_prop (me, "deleted",    deleted,    "desc of deleted");
}

/**
 * create
 */
LoopPtr
Loop::create (const std::string& losetup)
{
	std::vector<std::string> parts;

	explode_n (" :", losetup, parts, 12);

#if 0
	log_info ("parts: (%ld)\n", parts.size());
	for (auto i : parts) {
		std::cout << "\t" << i << std::endl;
	}
	log_info ("\n");
#endif

	//XXX validate all input, else throw()

	LoopPtr l (new Loop());
	l->weak = l;

	l->device     = parts[0];
	l->file_name  = parts[11];

	l->name = l->device;
	size_t index = l->name.find_last_of ('/');
	if (index != std::string::npos) {
		l->name = l->name.substr (index+1);
	}

	l->autoclear  = StringNum (parts[ 1]);
	l->file_inode = StringNum (parts[ 2]);
	l->file_major = StringNum (parts[ 3]);
	l->file_minor = StringNum (parts[ 4]);
	l->loop_major = StringNum (parts[ 5]);
	l->loop_minor = StringNum (parts[ 6]);
	l->offset     = StringNum (parts[ 7]);
	l->partscan   = StringNum (parts[ 8]);
	l->read_only  = StringNum (parts[ 9]);
	l->sizelimit  = StringNum (parts[10]);

	std::size_t len = l->file_name.size();
	if ((len > 10) && (l->file_name.substr (len-10) == " (deleted)")) {
		l->file_name.erase (len-10);
		l->deleted = true;
		//log_info ("%s is deleted\n", device.c_str());
	}

	//XXX tmp
	l->kernel_major = l->loop_major;
	l->kernel_minor = l->loop_minor;
	l->block_size   = 512;	//XXX kernel limit, but fs block size is likely to be bigger

	std::stringstream ss;
	ss << "[" << l->kernel_major << ":" << l->kernel_minor << "]";
	l->uuid = ss.str();

	return l;
}


/**
 * accept
 */
bool
Loop::accept (Visitor& v)
{
	LoopPtr l = std::dynamic_pointer_cast<Loop> (get_smart());
	if (!v.visit(l))
		return false;
	return visit_children(v);
}


/**
 * losetup
 */
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
	execute_command1 (command, output);
	if (output.empty())
		return false;

	return true;
}

/**
 * discover
 */
void
Loop::discover (ContainerPtr& top_level, std::queue<ContainerPtr>& probe_queue)
{
	std::vector <std::string> output;

	if (!losetup (output))
		return;

	for (auto line : output) {
		LoopPtr l = create (line);

		l->get_fd();

		// move to container::find_size or utils (or block::)
		off_t size;
		size = lseek (l->fd, 0, SEEK_END);
		l->bytes_size = size;

		ContainerPtr c(l);
		top_level->just_add_child(c);
		probe_queue.push(c);	// We need to probe
	}
}

/**
 * identify
 */
void
Loop::identify (ContainerPtr& top_level, const char* name, int fd, struct stat& UNUSED(st))
{
	//LOG_TRACE;

	off_t size;

	std::vector <std::string> output;

	losetup (output, name);		//XXX retval, exactly one reply

	//std::cout << output[0] << std::endl;

	LoopPtr l = create (output[0]);

	size = lseek (fd, 0, SEEK_END);

	l->device        = name;
	l->parent_offset = 0;
	l->bytes_size    = size;
	l->bytes_used    = 0;

	size_t i = l->device.find_last_of ('/');
	if (i == std::string::npos) {
		l->name = l->device;
	} else {
		l->name = l->device.substr (i+1);
	}

	l->block_size = 512;	//XXX granularity, or blocksize of backing-file fs/disk?

	std::stringstream ss;
	ss << "[" << l->loop_major << ":" << l->loop_minor << "]";
	l->uuid = ss.str();

	ContainerPtr c(l);
	top_level->just_add_child(c);
	main_app->queue_add_probe(c);	// queue the container for action
}


/**
 * get_actions
 */
std::vector<Action>
Loop::get_actions (void)
{
	// LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.loop", true },
	};

	std::vector<Action> parent_actions = Device::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

/**
 * perform_action
 */
bool
Loop::perform_action (Action action)
{
	if (action.name == "dummy.loop") {
		std::cout << "Loop perform: " << action.name << std::endl;
		return true;
	} else {
		return Device::perform_action (action);
	}
}


