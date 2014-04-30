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
#include "filesystem.h"
#include "log.h"
#include "misc.h"
#include "table.h"
#include "thread.h"
#include "utils.h"
#ifdef DP_DISK
#include "disk.h"
#endif
#ifdef DP_FILE
#include "file.h"
#endif
#ifdef DP_LOOP
#include "loop.h"
#endif
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
#ifndef DP_NO_THREAD
	{	// Scope
		for (auto& i : thread_queue) {
			i.join();	// Wait for things to finish
			//i.detach();	// Don't wait any longer
		}
		std::lock_guard<std::mutex> lock (thread_mutex);
		thread_queue.clear();
	}
#endif

	log_dtor ("dtor App");
}


bool
App::ask (QuestionPtr q)
{
	return_val_if_fail (q,false);

	log_code ("%s not implemented", __PRETTY_FUNCTION__);
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

	std::string command = "grep '^/dev' /proc/mounts";
	std::vector<std::string> output;

	int retval = execute_command_out (command, output);
	/* retval:
	 *	0 matches
	 *	1 no matches
	 *	1 invalid arguments
	 *	2 file doesn't exist
	 */

	for (unsigned int i = 0; i < output.size(); ++i) {
		std::string line = output[i];
		log_info ("line%d:", i);
		log_info (line);
	}

	return mounts.children.size();
}

#endif

void
App::queue_add_probe (ContainerPtr& item)
{
	return_if_fail (item);
	start_thread (std::bind (&App::process_queue_item, this, item));
}

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
	log_file ("file open: %d, '%s'", fd, device.c_str());

	struct stat st;
	int res = fstat (fd, &st);
	if (res < 0) {
		log_error ("stat on %s failed", device.c_str());	//XXX perror
		close (fd);
		log_file ("file close: %d", fd);
		return false;
	}

	if (false) {}
#ifdef DP_FILE
	else if (File::identify (parent, device, fd, st)) {}
#endif
#ifdef DP_LOOP
	else if (Loop::identify (parent, device, fd, st)) {}
#endif
#ifdef DP_DISK
	else if (Disk::identify (parent, device, fd, st)) {}
#endif
	else {
		log_error ("can't identify device: %s", device.c_str());
		close (fd);
		log_file ("file close: %d", fd);
		return false;
	}

	close (fd);
	log_file ("file close: %d", fd);
	return true;
}

ContainerPtr
App::scan (std::vector<std::string>& devices, scan_async_cb_t fn)
{
	LOG_TRACE;

#ifndef DP_NO_THREAD
	if (!thread_queue.empty()) {
		log_code ("only one scan at a time");
		return nullptr;
	}
#endif

	ContainerPtr top_level = Container::create();
	top_level->name = "dummy";

	if (devices.empty()) {
		// Check all device types at once
#ifdef DP_DISK
		start_thread (std::bind (&Disk::discover,     top_level));
#endif
#ifdef DP_FILE
		start_thread (std::bind (&File::discover,     top_level));
#endif
#ifdef DP_LOOP
		start_thread (std::bind (&Loop::discover,     top_level));
#endif
#ifdef DP_LVM
		start_thread (std::bind (&LvmGroup::discover, top_level));
#endif
	} else {
		//XXX need to spot Lvm Groups
		for (auto i : devices) {
			// Examine all the devices in parallel
			start_thread (std::bind (&App::identify_device, this, top_level, i));
		}
	}

#ifndef DP_NO_THREAD
	if (fn) {
		// Start a thread to watch the existing threads.
		// When the existing thread have finished notify the user with fn().
		std::thread ([=](){
			while (!thread_queue.empty()) {
				thread_queue.front().join();
				std::lock_guard<std::mutex> lock (thread_mutex);
				thread_queue.pop_front();
			}
			fn (top_level);
		}).detach();
	} else {
		// Wait for all the threads to finish before returning
		while (!thread_queue.empty()) {
			thread_queue.front().join();
			std::lock_guard<std::mutex> lock (thread_mutex);
			thread_queue.pop_front();
		}
	}
#else
	if (fn) {
		fn (top_level);
	}
#endif

	return top_level;
}

bool
App::process_queue_item (ContainerPtr item)
{
	return_val_if_fail (item,false);
	LOG_THREAD;

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


void
App::start_thread (std::function<void(void)> fn)
{
#ifdef DP_NO_THREAD
	fn();
#else
	std::lock_guard<std::mutex> lock (thread_mutex);
	thread_queue.push_back (std::thread (fn));
#endif
}


bool
App::open_uri (const std::string& uri)
{
	log_error ("Can't open uri: %s\n", uri.c_str());
	return false;
}

