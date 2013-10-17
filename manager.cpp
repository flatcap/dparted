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

#include <iostream>
#include <queue>
#include <string>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/major.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <sys/ioctl.h>

#include "container.h"
#include "manager.h"
#include "utils.h"
#include "log.h"

#include "table.h"
#include "misc.h"
#include "filesystem.h"
#include "lvm_group.h"

#include "probe_loop.h"
#include "probe_disk.h"
#include "probe_file.h"

#include "log_trace.h"
#include "dot.h"
#include "log_trace.h"

std::queue<DPContainer*> probe_queue;

/**
 * Manager
 */
Manager::Manager()
{
}

/**
 * ~Manager
 */
Manager::~Manager()
{
}


/**
 * queue_add_probe
 */
void queue_add_probe (DPContainer *item)
{
	if (!item)
		return;

	probe_queue.push (item);
	std::string s = get_size (item->parent_offset);
	//printf ("QUEUE: %s %s : %lld (%s)\n", item->name.c_str(), item->device.c_str(), item->parent_offset, s.c_str());
	//printf ("QUEUE has %lu items\n", probe_queue.size());
}

/**
 * mounts_get_list
 */
unsigned int mounts_get_list (DPContainer &mounts)
{
	std::string command;
	std::vector<std::string> output;
	std::string error;

	command = "grep '^/dev' /proc/mounts";
	execute_command (command, output);

	for (unsigned int i = 0; i < output.size(); i++) {
		std::string line = output[i];
		printf ("line%d:\n%s\n\n", i, line.c_str());
	}

	return mounts.children.size();
}


/**
 * probe
 */
DPContainer * probe (DPContainer &top_level, DPContainer *parent)
{
	//LOG_TRACE;
	const int bufsize = 66560;		//XXX round up to page size (69632)
	std::vector<unsigned char> buffer (bufsize);

	DPContainer *item = NULL;

	parent->open_device();
	parent->read_data (0, bufsize, &buffer[0]);	// check read num

	if ((item = Filesystem::probe (top_level, parent, &buffer[0], bufsize))) {
		return item;
	}

	if ((item = Table::probe (top_level, parent, &buffer[0], bufsize))) {
		return item;
	}

	if ((item = Misc::probe (top_level, parent, &buffer[0], bufsize))) {
		return item;
	}

	return NULL;
}

/**
 * main
 */
int main (int argc, char *argv[])
{
	log_init ("/dev/stdout");

	DPContainer top_level;
	top_level.name = "dummy";

	// Create the probes
	ProbeLoop pl;
	ProbeDisk pd;
	ProbeFile pf;

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
				pf.identify (top_level, argv[i], fd, st);
			} else if (S_ISBLK (st.st_mode)) {
				if (MAJOR (st.st_rdev) == LOOP_MAJOR) {
					pl.identify (top_level, argv[i], fd, st);
				} else {
					pd.identify (top_level, argv[i], fd, st);
				}
			} else {
				log_error ("can't probe something else\n");
			}
			close (fd);
		}
	} else {
		//pl.discover (top_level, probe_queue);
		pd.discover (top_level, probe_queue);
	}

	//XXX LVMGroup::find_devices (disks);

	// Process the probe_queue
	DPContainer *item = NULL;
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

#if 1
	for (auto c : top_level.children) {
		std::vector<DPContainer*> dummy;
		dummy.push_back(c);
		display_dot (dummy);
	}
#endif

	return 0;
}


