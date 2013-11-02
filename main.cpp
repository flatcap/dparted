/* Copyright (c) 2013 Richard Russon (FlatCap)
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Library General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <fcntl.h>
#include <gtkmm.h>
#include "gtkmm/main.h"
#include <sys/types.h>

#include <iostream>
#include <cstdlib>
#include <queue>
#include <string>
#include <vector>

#include <linux/major.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>

#include "app.h"
#include "container.h"
#include "disk.h"
#include "dot.h"
#include "dparted.h"
#include "file.h"
#include "filesystem.h"
#include "log.h"
#include "log_trace.h"
#include "loop.h"
#include "lvm_group.h"
#include "lvm_volume.h"
#include "md_group.h"
#include "misc.h"
#include "table.h"
#include "utils.h"
#include "volume.h"

std::queue<DPContainer*> probe_queue;

/**
 * queue_add_probe
 */
void
queue_add_probe (DPContainer *item)
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
mounts_get_list (DPContainer &mounts)
{
	std::string command;
	std::vector<std::string> output;
	std::string error;

	command = "grep '^/dev' /proc/mounts";
	execute_command (command, output);

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
DPContainer *
probe (DPContainer &top_level, DPContainer *parent)
{
	//LOG_TRACE;
	const int bufsize = 256*1024;
	std::vector<unsigned char> buffer (bufsize);

	DPContainer *item = nullptr;

	parent->open_device();
	parent->read_data (0, bufsize, &buffer[0]);	// check read num

	if ((item = Filesystem::probe (top_level, parent, &buffer[0], bufsize))) {
		//XXX need to check for all probed types
		//printf ("PROBE: parent %s, child %s\n", parent->type.back().c_str(), item->type.back().c_str());
#if 0
		if (parent->is_a ("whole")) {
			Whole *w = dynamic_cast<Whole*> (parent);
			for (auto i : w->segments) {
				//log_info ("add filesystem to %s - %s\n", i->name.c_str(), i->type.back().c_str());
				i->just_add_child (item);
			}
			if (parent->is_a ("lvm_volume")) {
				LvmVolume *v = dynamic_cast<LvmVolume*>(w);
				for (auto c : v->metadata) {
					w = dynamic_cast<Whole*> (c);
					if (w) {
						for (auto i : w->segments) {
							i->just_add_child (item);
						}
					}
				}
			}
		}
#endif
		return item;
	}

	if ((item = Table::probe (top_level, parent, &buffer[0], bufsize))) {
		return item;
	}

	if ((item = Misc::probe (top_level, parent, &buffer[0], bufsize))) {
		return item;
	}

	return nullptr;
}

/**
 * main
 */
int
main (int argc, char *argv[])
{
	//command line
	// -a	app
	// -l	list
	// -d	dot

	bool app  = false;
	bool dot  = false;
	bool list = false;

	if (argc > 1) {
		std::string arg = argv[1];
		if (arg[0] == '-') {
			for (auto c : arg) {
				switch (c) {
					case '-':              break;
					case 'a': app  = true; break;
					case 'd': dot  = true; break;
					case 'l': list = true; break;
					default:
						  printf ("unknown option '%c'\n", c);
						  break;
				}
			}
			argc--;
			argv++;
		}
	}

	log_init ("/dev/stdout");

	DPContainer top_level;
	top_level.name = "dummy";

	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			struct stat st;
			int res = -1;
			int fd = -1;

			fd = open (argv[i], O_RDONLY | O_CLOEXEC);
			if (fd < 0) {
				log_debug ("can't open file %s\n", argv[i]);
				continue;
			}

			res = fstat (fd, &st);
			if (res < 0) {
				log_debug ("stat on %s failed\n", argv[i]);
				close (fd);
				continue;
			}

			if (S_ISREG (st.st_mode) || S_ISDIR (st.st_mode)) {
				File::identify (top_level, argv[i], fd, st);
			} else if (S_ISBLK (st.st_mode)) {
				if (MAJOR (st.st_rdev) == LOOP_MAJOR) {
					Loop::identify (top_level, argv[i], fd, st);
				} else {
					//Disk::identify (top_level, argv[i], fd, st);
				}
			} else {
				log_error ("can't probe something else\n");
			}
			close (fd);
		}
	} else {
		Loop::discover (top_level, probe_queue);
		//Disk::discover (top_level, probe_queue);
	}

	// Process the probe_queue
	DPContainer *item = nullptr;
	//XXX deque?
	while ((item = probe_queue.front())) {
		probe_queue.pop();

		//std::cout << "Item: " << item << "\n";

		DPContainer *found = probe (top_level, item);
		if (found) {
			//item->add_child (found);
			//std::cout << "\tFound: " << found << "\n";
			//probe_queue.push (found);
		}
	}

	LvmGroup::discover (top_level);
	//MdGroup::discover (top_level);

#if 1
	// Process the probe_queue
	//XXX deque?
	while ((item = probe_queue.front())) {
		probe_queue.pop();

		//std::cout << "Item: " << item << "\n";

		DPContainer *found = probe (top_level, item);
		if (found) {
			//item->add_child (found);
			//std::cout << "\tFound: " << found << "\n";
			//probe_queue.push (found);
		}
	}
#endif

	if (list) {
		log_info ("------------------------------------------------------------\n");
		top_level.dump_objects();
		log_info ("------------------------------------------------------------\n");
	}

	if (dot) {
		display_dot (top_level.children);
	}
#if 0
	if (argc != 1) {
		display_dot (top_level.children);
	} else {
		for (auto c : top_level.children) {
			std::vector<DPContainer*> dummy;
			dummy.push_back(c);
			display_dot (dummy);
		}
	}
#endif
	int retval = 0;

	if (app) {
		App app (&top_level);
		retval =  app.run (1, argv);		//XXX argc
	}

	return retval;
}


