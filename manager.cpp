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

#include "container.h"
#include "manager.h"
#include "utils.h"
#include "log.h"

#include "table.h"
#include "misc.h"
#include "disk.h"
#include "filesystem.h"
#include "loop.h"
#include "lvm_group.h"

#include "probe_loop.h"
#include "probe_disk.h"

#include "dot.h"

std::queue<DPContainer*> probe_queue;

/**
 * Manager
 */
Manager::Manager () :
	m_c (NULL)
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
	//printf ("QUEUE: %s %s - %lld (%s)\n", item->name.c_str(), item->device.c_str(), item->parent_offset, s.c_str());
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
DPContainer * probe (DPContainer *parent)
{
	const int bufsize = 66560;
	std::vector<unsigned char> buffer (bufsize);

	DPContainer *item = NULL;

	parent->open_device();
	parent->read_data (0, bufsize, &buffer[0]);	// check read num

	if ((item = Filesystem::probe (parent, &buffer[0], bufsize))) {
		return item;
	}

	if ((item = Table::probe (parent, &buffer[0], bufsize))) {
		return item;
	}

	if ((item = Misc::probe (parent, &buffer[0], bufsize))) {
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

	ProbeLoop pl;
	pl.discover();

	ProbeDisk pd;
	pd.discover();

	display_dot (pl.get_children());

#if 0
	Extended::probe   (DPContainer *parent, long long offset, long long size)
	Filesystem::probe (DPContainer *parent, unsigned char *buffer, int bufsize)
	Gpt::probe        (DPContainer *parent, unsigned char *buffer, int bufsize)
	LVMTable::probe   (DPContainer *parent, unsigned char *buffer, int bufsize)
	Misc::probe       (DPContainer *parent, unsigned char *buffer, int bufsize)
	Msdos::probe      (DPContainer *parent, unsigned char *buffer, int bufsize)
	Table::probe      (DPContainer *parent, unsigned char *buffer, int bufsize)
#endif


#if 0
	typedef unsigned int (*find_devices) (DPContainer &list);

	find_devices l = Loop::find_devices;	// XXX static, so we get away with this
	find_devices d = Disk::find_devices;

	printf ("l = %p\n", l);
	printf ("d = %p\n", d);

	{
		DPContainer disks;
		DPContainer *item = NULL;

		//Disk::find_devices (disks);
		//Loop::find_devices (disks);
		l (disks);

		for (auto child : disks.children) {
			queue_add_probe (child);
		}

		while ((item = probe_queue.front())) {
			probe_queue.pop();

			//std::cout << "Item: " << item << "\n";
			//printf ("queue has %lu items\n", probe_queue.size());
			DPContainer *found = probe (item);
			if (found) {
				item->add_child (found);
				//printf ("add_child %p [%s/%s] -> %p [%s/%s]\n", item, item->type.back().c_str(), item->name.c_str(), found, found->type.back().c_str(), found->name.c_str());
			} //RAR else what?
			//printf ("queue has %lu items\n", probe_queue.size());
		}

		LVMGroup::find_devices (disks);

		while ((item = probe_queue.front())) {
			probe_queue.pop();

			DPContainer *found = probe (item);
			if (found) {
				item->add_child (found);
				//printf ("add_child %p [%s/%s] -> %p [%s/%s]\n", item, item->type.back().c_str(), item->name.c_str(), found, found->type.back().c_str(), found->name.c_str());
			} //RAR else what?
		}

		if (probe_queue.size() > 0) {
			printf ("Queue still contains work (%lu items)\n", probe_queue.size());
		}

		//dump_dot();

	}
	DPContainer::dump_leaks();
#endif
	return 0;
}


