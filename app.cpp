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


void
App::queue_add_probe (ContainerPtr& item)
{
	return_if_fail (item);

	probe_queue.push (item);
	std::string s = get_size (item->parent_offset);
	log_info ("QUEUE: %s %s : %ld (%s)", item->name.c_str(), item->device.c_str(), item->parent_offset, s.c_str());
	log_info ("QUEUE has %lu items", probe_queue.size());
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

#if 0
bool
App::probe (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize)
{
	return_val_if_fail (parent,  false);
	return_val_if_fail (buffer,  false);
	return_val_if_fail (bufsize, false);
	LOG_TRACE;

	if (Filesystem::probe (parent, buffer, bufsize))
		return true;

	if (Table::probe (parent, buffer, bufsize))
		return true;

	if (Misc::probe (parent, buffer, bufsize))
		return true;

	return false;
}

ContainerPtr
App::scan (const std::vector<std::string>& files)
{
	top_level = Container::create();	// Trash the old value
	top_level->name = "dummy";

	if (files.size() > 0) {
		for (auto f : files) {
			struct stat st;
			int res = -1;
			int fd = -1;

			fd = open (f.c_str(), O_RDONLY | O_CLOEXEC);
			if (fd < 0) {
				log_debug ("can't open file %s", f.c_str());
				continue;
			}

			res = fstat (fd, &st);
			if (res < 0) {
				log_debug ("stat on %s failed", f.c_str());
				close (fd);
				continue;
			}

			if (S_ISREG (st.st_mode) || S_ISDIR (st.st_mode)) {
				File::identify (top_level, f.c_str(), fd, st);
			} else if (S_ISBLK (st.st_mode)) {
				if (MAJOR (st.st_rdev) == LOOP_MAJOR) {
					Loop::identify (top_level, f.c_str(), fd, st);
				} else {
					//Gpt::identify (top_level, f.c_str(), fd, st);
				}
			} else {
				log_error ("can't probe something else");
			}
			close (fd);
		}
	} else {
		Loop::discover (top_level, probe_queue);
		//Gpt::discover (top_level, probe_queue);
	}

	// Process the probe_queue
	ContainerPtr item;
	while (!probe_queue.empty()) {
		item = probe_queue.front();
		probe_queue.pop();

		log_debug ("Item: %s", item->dump().c_str());

		std::uint64_t bufsize = item->bytes_size;
		std::uint8_t* buffer  = item->get_buffer (0, bufsize);

		if (!buffer) {
			log_error ("can't get buffer");
			continue;
		}

		if (!probe (item, buffer, bufsize)) {
			//XXX LOG -- should be logged upstream
			break;
		}
	}

#ifdef DP_LVM
	LvmGroup::discover (top_level);
#endif
	//MdGroup::discover (top_level);

	// Process the probe_queue
	while (!probe_queue.empty()) {
		item = probe_queue.front();
		probe_queue.pop();

		log_debug ("Item: %s", item->dump().c_str());

		std::uint64_t bufsize = item->bytes_size;
		std::uint8_t* buffer  = item->get_buffer (0, bufsize);

		if (!buffer) {
			log_error ("can't get buffer");
			continue;
		}

		if (!probe (item, buffer, bufsize)) {
			//XXX LOG -- should be logged upstream
			break;
		}
	}
	return top_level;
}

void
App::scan_async_do (const std::vector<std::string>& files, scan_async_cb_t callback)
{
	return_if_fail (callback);

	auto t1 = std::chrono::steady_clock::now();
	std::thread::id thread_id = std::this_thread::get_id();
	std::uint64_t tid = (std::uint64_t) *(reinterpret_cast<std::uint64_t*> (&thread_id));	// Just get me a number
	log_thread ("thread id: %ld started", tid);

	ContainerPtr c = scan (files);

	auto t2 = std::chrono::steady_clock::now();
	auto span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	log_thread ("thread id: %ld stopped (%0.3g seconds)", tid, span.count());

	callback (c);
}

bool
App::scan_async (const std::vector<std::string>& files, scan_async_cb_t callback)
{
	return_val_if_fail (callback, false);

	std::thread thread (std::bind (&App::scan_async_do, this, std::placeholders::_1, std::placeholders::_2), files, callback);

	thread.detach();	// I don't want to wait
	return true;
}

#endif

ContainerPtr
App::scan (std::vector<std::string>& devices)
{
	LOG_TRACE;

	ContainerPtr top_level = Container::create();

	if (devices.size() > 0) {
		identify (top_level, devices);
	} else {
		discover (top_level);
	}

	return top_level;
}

bool
App::identify_device (ContainerPtr parent, std::string& device)
{
	return_val_if_fail (parent, false);
	return_val_if_fail (!device.empty(), false);
	LOG_TRACE;

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

void
App::identify (ContainerPtr parent, std::vector<std::string>& devices)
{
	return_if_fail(parent);
	LOG_TRACE;

	//XXX need to spot Lvm Groups
	for (auto i : devices) {
		// Examine all the devices in parallel
		std::thread (std::bind (&App::identify_device, this, parent, i)).detach();
	}
}

void
App::discover (ContainerPtr parent)
{
	return_if_fail (parent);
	LOG_TRACE;

	// Check all device types at once
	std::thread (std::bind (&Disk::discover,     parent)).detach();
	std::thread (std::bind (&File::discover,     parent)).detach();
	std::thread (std::bind (&Loop::discover,     parent)).detach();
	std::thread (std::bind (&LvmGroup::discover, parent)).detach();
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

bool
App::process_queue (void)
{
	LOG_TRACE;

	while (!probe_queue.empty()) {
		ContainerPtr item = probe_queue.front();
		probe_queue.pop();

		// Examine all the items in parallel
		//std::thread (process_queue_item, item).detach();
	}

	return (probe_queue.empty());
}


