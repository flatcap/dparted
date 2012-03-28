/* Copyright (c) 2012 Richard Russon (FlatCap)
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
#include <sys/types.h>

#include <cstdlib>
#include <queue>
#include <string>
#include <vector>

#include "container.h"
#include "disk.h"
#include "dparted.h"
#include "filesystem.h"
#include "log.h"
#include "loop.h"
#include "lvm_group.h"
#include "misc.h"
#include "table.h"
#include "utils.h"
#include "volume.h"

std::queue<DPContainer*> probe_queue;

/**
 * queue_add_probe
 */
void queue_add_probe (DPContainer *item)
{
	if (!item)
		return;

	probe_queue.push (item);
	std::string s = get_size (item->parent_offset);
	//log_debug ("QUEUE: %s %s - %lld (%s)\n", item->name.c_str(), item->device.c_str(), item->parent_offset, s.c_str());
	//log_info ("QUEUE has %lu items\n", probe_queue.size());
}

/**
 * mounts_get_list
 */
unsigned int mounts_get_list (DPContainer &mounts)
{
	std::string command;
	std::vector<std::string> output;
	std::string error;
	unsigned int i;

	command = "grep '^/dev' /proc/mounts";
	execute_command (command, output);

	for (i = 0; i < output.size(); i++) {
		std::string line = output[i];
		log_debug ("line%d:\n%s\n\n", i, line.c_str());
	}

	return mounts.children.size();
}

/**
 * main
 */
int main (int argc, char *argv[])
{
	DPContainer disks;
	DPContainer *item = NULL;

	log_init ("/dev/stderr");

	disks.name = "container";	//XXX dummy

	//Disk::find_devices (disks);
	Loop::find_devices (disks);

	unsigned char *buffer = NULL;
	int bufsize = 32768;	//RAR 4096;
	unsigned int j;

	for (j = 0; j < disks.children.size(); j++) {
		queue_add_probe (disks.children[j]);
	}

	buffer = (unsigned char*) malloc (bufsize);
	if (!buffer)
		return 1;

	while ((item = probe_queue.front())) {
#if 0
		log_debug ("queued item: '%s'\n", item->name.c_str());
		std::string s1 = get_size (item->parent_offset);
		std::string s2 = get_size (item->bytes_size);
		log_debug ("\tdevice     = %s\n",        item->device.c_str());
		log_debug ("\toffset     = %lld (%s)\n", item->parent_offset, s1.c_str());
		log_debug ("\ttotal size = %lld (%s)\n", item->bytes_size, s2.c_str());
		log_debug ("\n");
#endif
		probe_queue.pop();

		item->read_data (0, bufsize, buffer);

		if (Filesystem::probe (item, buffer, bufsize)) {
			continue;
		}

		if (Table::probe (item, buffer, bufsize)) {
			continue;
		}

		if (Misc::probe (item, buffer, bufsize)) {
			continue;
		}
	}

	LVMGroup::find_devices (disks);

	while ((item = probe_queue.front())) {
#if 0
		log_debug ("queued item: '%s'\n", item->name.c_str());
		std::string s1 = get_size (item->parent_offset);
		std::string s2 = get_size (item->bytes_size);
		log_debug ("\tdevice     = %s\n",        item->device.c_str());
		log_debug ("\toffset     = %lld (%s)\n", item->parent_offset, s1.c_str());
		log_debug ("\ttotal size = %lld (%s)\n", item->bytes_size, s2.c_str());
		log_debug ("\n");
#endif
		probe_queue.pop();

		item->read_data (0, bufsize, buffer);

		if (Filesystem::probe (item, buffer, bufsize)) {
			continue;
		}

		if (Table::probe (item, buffer, bufsize)) {
			continue;
		}

		if (Misc::probe (item, buffer, bufsize)) {
			continue;
		}
	}

	if (probe_queue.size() > 0) {
		log_error ("Queue still contains work (%lu items)\n", probe_queue.size());
	}

#if 1
	std::cout << DPContainer::dump_objects();
#else
	Gtk::Main kit (argc, argv);

	DParted d;
	d.set_data (&disks);

	Gtk::Main::run (d);
#endif

	log_close();
	return 0;
}

