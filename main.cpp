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

	if (!parent)
		return nullptr;

	DPContainer *item = nullptr;

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
 * main
 */
int
main (int argc, char *argv[])
{
	//command line
	// -a	app
	// -l	list
	// -d	dot
	// -s	separate dot diagrams

	bool app      = false;
	bool dot      = false;
	bool list     = false;
	bool separate = false;

	if (argc > 1) {
		std::string arg = argv[1];
		if (arg[0] == '-') {
			for (auto c : arg) {
				switch (c) {
					case '-':                  break;
					case 'a': app      = true; break;
					case 'd': dot      = true; break;
					case 'l': list     = true; break;
					case 's': separate = true; break;
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
	while (!probe_queue.empty()) {
		item = probe_queue.front();
		probe_queue.pop();

		//std::cout << "Item: " << item << "\n";

		DPContainer *found = probe (top_level, item);
		if (found) {
			//item->add_child (found);
			//std::cout << "\tFound: " << found << "\n";
			//probe_queue.push (found);
		} else {
			//XXX LOG
			break;
		}
	}

	//LvmGroup::discover (top_level);
	//MdGroup::discover (top_level);

#if 1
	// Process the probe_queue
	//XXX deque?
	while (!probe_queue.empty()) {
		item = probe_queue.front();
		probe_queue.pop();

		//std::cout << "Item: " << item << "\n";

		DPContainer *found = probe (top_level, item);
		if (found) {
			//item->add_child (found);
			//std::cout << "\tFound: " << found << "\n";
			//probe_queue.push (found);
		} else {
			//XXX LOG
			break;
		}
	}
#endif

	if (list) {
		log_info ("------------------------------------------------------------\n");
		top_level.dump_objects();
		log_info ("------------------------------------------------------------\n");
	}

	if (dot) {
		if (separate) {
			for (auto c : top_level.children) {
				std::vector<DPContainer*> dummy;
				dummy.push_back(c);
				display_dot (dummy);
			}
		} else {
			display_dot (top_level.children);
		}
	}

	int retval = 0;

	if (app) {
		App app (&top_level);
		retval =  app.run (1, argv);		//XXX argc
	}

	log_close();
	return retval;
}


