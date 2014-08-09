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
#include <algorithm>

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
#include "utils.h"
#include "list_visitor.h"
#include "timeline.h"

AppPtr main_app;

App::App (void)
{
	LOG_CTOR;
}

App::~App()
{
#ifdef DP_THREADED
	{	// Scope
		for (auto& i : thread_queue) {
			i.join();	// Wait for things to finish
			// i.detach();	// Don't wait any longer
		}
		std::lock_guard<std::mutex> lock (thread_mutex);
		thread_queue.clear();
	}
#endif

	LOG_DTOR;
}


bool
App::ask (QuestionPtr q)
{
	return_val_if_fail (q, false);

	log_code ("%s not implemented", __PRETTY_FUNCTION__);
	return false;
}


bool
App::set_config (const std::string& filename)
{
	ConfigFilePtr cf;

	cf = ConfigFile::read_file (filename);
	if (!cf) {
		// notify the user
		return false;
	}

	if (config_file) {
		// if modified ask user if they're sure
	}

	config_file = cf;
	// cf->dump_config();

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
	start_thread (std::bind (&App::process_queue_item, this, item), "App::process_queue_item");
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

#ifdef DP_THREADED
	if (!thread_queue.empty()) {
		log_code ("only one scan at a time");
		return nullptr;
	}
#endif

	ContainerPtr top_level = Container::create();	// Dropping any old results
	top_level->name = "dummy";
	timeline = Timeline::create (top_level);

	if (devices.empty()) {
		// Check all device types at once
#ifdef DP_DISK
		start_thread (std::bind (&Disk::discover,     top_level), "Disk::discover");
#endif
#ifdef DP_FILE
		start_thread (std::bind (&File::discover,     top_level), "File::discover");
#endif
#ifdef DP_LOOP
		start_thread (std::bind (&Loop::discover,     top_level), "Loop::discover");
#endif
#ifdef DP_LVM
		start_thread (std::bind (&LvmGroup::discover, top_level), "LvmGroup::discover");
#endif
	} else {
		//XXX need to spot Lvm Groups
		for (auto& i : devices) {
			// Examine all the devices in parallel
			start_thread (std::bind (&App::identify_device, this, top_level, i), "App::identify_device");
		}
	}

#ifdef DP_THREADED
	if (fn) {
		// Start a thread to watch the existing threads.
		// When the existing thread have finished notify the user with fn().
		std::thread ([=](){
			log_thread_start ("Waiting for threads to finish");
			while (!thread_queue.empty()) {
				thread_queue.front().join();
				std::lock_guard<std::mutex> lock (thread_mutex);
				thread_queue.pop_front();
			}
			fn (top_level);
			log_thread_end ("Threads have finished");
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
	return_val_if_fail (item, false);
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


void
App::start_thread (std::function<void(void)> fn, const char* desc)
{
	//XXX use of desc is clumsy, but will do for now
#ifdef DP_THREADED
	std::lock_guard<std::mutex> lock (thread_mutex);
	thread_queue.push_back (
		std::thread ([fn, desc]() {
			log_thread_start ("thread started: %s", desc);
			fn();
			log_thread_end   ("thread ended:   %s", desc);
		})
	);
#else
	fn();
#endif
}


bool
App::open_uri (const std::string& uri)
{
	log_error ("Can't open uri: %s", uri.c_str());
	return false;
}


std::vector<ContainerWeak> all_children;
std::mutex mutex_children;

int
count_containers (const ContainerPtr& c)
{
	if (!c)
		return 0;

	const auto& children = c->get_children();

	int count = 1;
	for (const auto& i : children) {
		count += count_containers(i);
	}

	return count;
}

void
dump_children (void)
{
	for (auto c : all_children) {
		if (c.expired()) {
			printf ("X");
		} else {
			printf ("O");
		}
	}
}

void
tidy_children (void)
{
	// log_info ("TIDY ---------------------------------------------------------------------------------------------------------------------");
	// log_info ("\t%ld children", all_children.size());
	int live = 0;
	int dead = 0;
	for (auto c : all_children) {
		if (c.expired()) {
			dead++;
		} else {
			live++;
		}
	}
	// log_info ("\t\t%d live", live);
	// log_info ("\t\t%d dead", dead);

	// printf ("REMOVE_IF\n");
	// printf ("\t"); dump_children(); printf ("\n");
	auto new_end = std::remove_if (std::begin(all_children), std::end(all_children), [](ContainerWeak& c) { return c.expired(); });
	// printf ("\t"); dump_children(); printf ("\n");

	// log_info ("\tcheck start");
	for (auto it = begin(all_children); it != new_end; ++it) {
		if ((*it).expired()) {
			log_code ("\tdead link missed");
		}
	}

#if 0
	for (auto it = new_end; it != end(all_children); ++it) {
		if (!(*it).expired()) {
			log_info ("\t\tlive link deleted");
		}
	}
#endif
	// log_info ("\tcheck end");

	int before = all_children.size();
	all_children.erase (new_end, std::end(all_children));
	int after  = all_children.size();
	if (before != after) {
		log_info ("\ttidied %d children", (before-after));
	}

	live = 0;
	dead = 0;
	for (auto c : all_children) {
		if (c.expired()) {
			dead++;
		} else {
			live++;
		}
	}
	if (dead != 0) {
		log_code ("\t%ld children (after)", all_children.size());
		log_code ("\t\t%d live", live);
		log_code ("\t\t%d dead", dead);
	}

}

void
add_child (int i)
{
	int size = all_children.size();
	if (size == 0) {
		log_code ("no children left");
		return;
	}

	int pick = rand() % size;
	ContainerPtr parent = all_children[pick].lock();
	if (!parent) {
		log_code ("child %d is dead", pick);
		return;
	}

	ContainerPtr c = Container::create();
	{
	std::lock_guard<std::mutex> lock (mutex_children);
	all_children.push_back(c);
	}
	std::string s = "name" + std::to_string (i) + " ";
	c->name = s;
	parent->add_child(c, false);
}

void
alter_child (void)
{
	int size = all_children.size();
	if (size == 0) {
		log_code ("no children left");
		return;
	}

	int pick = rand() % size;
	ContainerPtr child = all_children[pick].lock();
	if (!child) {
		log_code ("child %d is dead", pick);
		return;
	}

	child->name += 'M';
}

void
delete_child (void)
{
	int size = all_children.size();
	if (size == 0) {
		log_code ("no children left");
		return;
	}

	int pick = (rand() % (size-1)) + 1;	// Skip 0

	ContainerPtr child = all_children[pick].lock();
	if (!child) {
		log_code ("child %d is dead", pick);
		return;
	}

	ContainerPtr parent = child->get_parent();
	if (!parent) {
		log_code ("can't delete top-level");
		return;
	}

	{
	std::lock_guard<std::mutex> lock (mutex_children);
	printf ("\tdeleting: %d children\n", count_containers(child));
	printf ("%dC/%ldV children\n", count_containers(all_children[0].lock()), all_children.size());
	parent->delete_child (child);
	all_children.erase (std::begin(all_children)+pick);
	tidy_children();
	printf ("%dC/%ldV children\n", count_containers(all_children[0].lock()), all_children.size());
	}
}

void
App::wait_for_threads (void)
{
#ifdef DP_THREADED
	// printf ("Waiting for threads to finish\n");
	while (!thread_queue.empty()) {
		thread_queue.front().join();
		std::lock_guard<std::mutex> lock (thread_mutex);
		thread_queue.pop_front();
	}
#endif
}

void
App::test (void)
{
	ContainerPtr c = Container::create();
	c->name = "top";
	all_children.push_back(c);

	for (int i = 0; i < 99; i++) {
#if 1
		start_thread (std::bind (add_child, i), "new");
#else
		add_child(i);
#endif
	}

	wait_for_threads();
	// printf ("START: %d\n", count_containers(c));

#if 0
	for (int i = 0; i < 100; i++) {
		start_thread (std::bind (alter_child), "alter");
	}
#endif

#if 0
	for (int i = 0; i < 3; i++) {
		// start_thread (std::bind (delete_child), "delete");
		delete_child();
	}
#endif

	// wait_for_threads();

	// printf ("Threads have finished\n");
	// run_list(c);
#if 1
	// tidy_children();
	printf ("%dC/%ldV children\n", count_containers(c), all_children.size());
#endif
}


bool
App::adjust_timeline (int amount)
{
	// LOG_TRACE;
	return timeline->adjust (amount);
}

TimelinePtr
App::get_timeline (void)
{
	return timeline;
}


