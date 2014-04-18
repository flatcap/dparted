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

#include <string>
#include <sstream>
#include <ratio>
#include <chrono>
#include <thread>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/major.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>

#include "app.h"
#include "disk.h"
#include "file.h"
#include "filesystem.h"
#include "log.h"
#include "loop.h"
#include "misc.h"
#include "table.h"
#include "utils.h"
#ifdef DP_LVM
#include "lvm_group.h"
#endif

AppPtr main_app;

App::App (void)
{
	log_ctor ("ctor App");
}

App::~App()
{
	log_dtor ("dtor App");
}


bool
App::ask (QuestionPtr q)
{
	return_val_if_fail (q, false);

	log_debug (q->title);
	log_debug (q->question);
	std::stringstream ss;
	ss << "\t";
	for (auto a : q->answers) {
		ss << a << ' ';
	}
	log_debug (ss);
	return false;
}

bool
App::notify (Message& UNUSED(m))
{
	// Might need to queue these until we're ready to confront the user
	return false;
}


bool
App::set_config (const std::string& filename)
{
	ConfigFilePtr cf;

	cf = ConfigFile::read_file (filename);
	if (!cf) {
		//notify the user
		return false;
	}

	if (config_file) {
		//if modified ask user if they're sure
	}

	config_file = cf;
	//cf->dump_config();

	return true;
}

ConfigFilePtr
App::get_config (void)
{
	return config_file;
}


#if 0
unsigned int
mounts_get_list (ContainerPtr& mounts)
{
	return_val_if_fail (mounts, 0);

	std::string command;
	std::vector<std::string> output;
	std::string error;

	command = "grep '^/dev' /proc/mounts";
	execute_command_out (command, output);

	for (unsigned int i = 0; i < output.size(); ++i) {
		std::string line = output[i];
		log_info ("line%d:", i);
		log_info (line);
	}

	return mounts.children.size();
}

#endif

bool
App::identify_device (ContainerPtr parent, std::string& device)
{
	return_val_if_fail (parent, false);
	return_val_if_fail (!device.empty(), false);
	LOG_THREAD;

	int fd = -1;		//XXX write a RAII wrapper around this

	fd = open (device.c_str(), O_RDONLY | O_CLOEXEC);
	if (fd < 0) {
		log_error ("can't open file %s", device.c_str());	//XXX perror
		return false;
	}

	struct stat st;
	int res = fstat (fd, &st);
	if (res < 0) {
		log_error ("stat on %s failed", device.c_str());	//XXX perror
		close (fd);
		return false;
	}

	     if (File::identify (parent, device, fd, st)) {}
	else if (Loop::identify (parent, device, fd, st)) {}
	else if (Disk::identify (parent, device, fd, st)) {}
	else {
		log_error ("can't identify device: %s", device.c_str());
		close (fd);
		return false;
	}

	close (fd);
	return true;
}

ContainerPtr
App::scan (std::vector<std::string>& devices)
{
	LOG_TRACE;

	ContainerPtr top_level = Container::create();

	if (devices.empty()) {
		// Check all device types at once
		std::thread (std::bind (&Disk::discover,     top_level)).detach();
		std::thread (std::bind (&File::discover,     top_level)).detach();
		std::thread (std::bind (&Loop::discover,     top_level)).detach();
#ifdef DP_LVM
		std::thread (std::bind (&LvmGroup::discover, top_level)).detach();
#endif
	} else {
		//XXX need to spot Lvm Groups
		for (auto i : devices) {
			// Examine all the devices in parallel
			std::thread (std::bind (&App::identify_device, this, top_level, i)).detach();
		}
	}

	return top_level;
}

bool
App::process_queue_item (ContainerPtr item)
{
	return_val_if_fail(item,false);
	LOG_TRACE;

	std::uint64_t bufsize = item->bytes_size;
	std::uint8_t* buffer  = item->get_buffer (0, bufsize);

	if (!buffer) {
		log_error ("can't get buffer");
		return false;
	}

	if (Filesystem::probe (item, buffer, bufsize))
		return true;

	if (Table::probe (item, buffer, bufsize))
		return true;

	if (Misc::probe (item, buffer, bufsize))
		return true;

	return false;
}

