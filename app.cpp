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

#include <iostream>
#include <string>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <linux/major.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>

#include "app.h"
#include "utils.h"
#include "filesystem.h"
#include "table.h"
#include "misc.h"
#include "file.h"
#include "loop.h"
#include "log.h"
#include "lvm_group.h"

AppPtr main_app;

/**
 * App
 */
App::App (void)
{
}

/**
 * ~App
 */
App::~App()
{
}


/**
 * ask
 */
bool
App::ask (QuestionPtr q)
{
	std::cout << q->title << std::endl;
	std::cout << q->question << std::endl;
	std::cout << '\t';
	for (auto a : q->answers) {
		std::cout << a << " ";
	}
	std::cout << '\n';
	return false;
}

/**
 * notify
 */
bool
App::notify (Message& m)
{
	// Might need to queue these until we're ready to confront the user
	return false;
}


/**
 * set_config
 */
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

/**
 * get_config
 */
ConfigFilePtr
App::get_config (void)
{
	return config_file;
}


/**
 * queue_add_probe
 */
void
App::queue_add_probe (ContainerPtr& item)
{
	if (!item)
		return;

	probe_queue.push (item);
	std::string s = get_size (item->parent_offset);
	//log_info ("QUEUE: %s %s : %lld (%s)\n", item->name.c_str(), item->device.c_str(), item->parent_offset, s.c_str());
	//log_info ("QUEUE has %lu items\n", probe_queue.size());
}


#if 0
/**
 * mounts_get_list
 */
unsigned int
mounts_get_list (ContainerPtr& mounts)
{
	std::string command;
	std::vector<std::string> output;
	std::string error;

	command = "grep '^/dev' /proc/mounts";
	execute_command1 (command, output);

	for (unsigned int i = 0; i < output.size(); i++) {
		std::string line = output[i];
		log_info ("line%d:\n%s\n\n", i, line.c_str());
	}

	return mounts.children.size();
}

#endif
/**
 * probe
 */
ContainerPtr
App::probe (ContainerPtr& top_level, ContainerPtr& parent)
{
	//LOG_TRACE;

	if (!top_level || !parent)
		return nullptr;

	ContainerPtr item;

	if ((item = Filesystem::probe (top_level, parent))) {
		return item;
	}

	if ((item = Table::probe (top_level, parent))) {
		return item;
	}

	if ((item = Misc::probe (top_level, parent))) {
		return item;
	}

	return nullptr;
}


/**
 * scan
 */
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
				log_debug ("can't open file %s\n", f.c_str());
				continue;
			}

			res = fstat (fd, &st);
			if (res < 0) {
				log_debug ("stat on %s failed\n", f.c_str());
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
				log_error ("can't probe something else\n");
			}
			close (fd);
		}
	} else {
		Loop::discover (top_level, probe_queue);
		//Gpt::discover (top_level, probe_queue);
	}

	// Process the probe_queue
	ContainerPtr item;
	//XXX deque?
	while (!probe_queue.empty()) {
		item = probe_queue.front();
		probe_queue.pop();

		//std::cout << "Item: " << item << "\n";

		ContainerPtr found = probe (top_level, item);
		if (found) {
			//std::cout << "top_level = " << top_level->get_children().size() << std::endl;
			//item->add_child (found);
			//std::cout << "\tFound: " << found << "\n";
			//probe_queue.push (found);
		} else {
			//XXX LOG
			break;
		}
		//std::cout << std::endl;
	}

	LvmGroup::discover (top_level);
	//MdGroup::discover (top_level);

	// Process the probe_queue
	//XXX deque?
	while (!probe_queue.empty()) {
		item = probe_queue.front();
		probe_queue.pop();

		//std::cout << "Item: " << item << "\n";

		ContainerPtr found = probe (top_level, item);
		if (found) {
			top_level->just_add_child (found);
			//item->add_child (found);
			//std::cout << "\tFound: " << found << "\n";
			//probe_queue.push (found);
		} else {
			//XXX LOG
			break;
		}
	}
	return top_level;
}

