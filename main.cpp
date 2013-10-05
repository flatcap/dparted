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
#include "gtkmm/main.h"
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
	{
	DPContainer disks;
	DPContainer *item = NULL;
	unsigned int j;

	log_init ("/dev/stderr");

	//Disk::find_devices (disks);
	Loop::find_devices (disks);

	for (j = 0; j < disks.children.size(); j++) {
		queue_add_probe (disks.children[j]);
	}

	while ((item = probe_queue.front())) {
		probe_queue.pop();

		//std::cout << "Item: " << item << "\n";
		//log_debug ("queue has %lu items\n", probe_queue.size());
		DPContainer *found = probe (item);
		if (found) {
			item->add_child (found);
			//log_debug ("add_child %p [%s/%s] -> %p [%s/%s]\n", item, item->type.back().c_str(), item->name.c_str(), found, found->type.back().c_str(), found->name.c_str());
		} //RAR else what?
		//log_debug ("queue has %lu items\n", probe_queue.size());
	}

#if 1
	LVMGroup::find_devices (disks);

	while ((item = probe_queue.front())) {
		probe_queue.pop();

		DPContainer *found = probe (item);
		if (found) {
			item->add_child (found);
			//log_debug ("add_child %p [%s/%s] -> %p [%s/%s]\n", item, item->type.back().c_str(), item->name.c_str(), found, found->type.back().c_str(), found->name.c_str());
		} //RAR else what?
	}

	if (probe_queue.size() > 0) {
		log_error ("Queue still contains work (%lu items)\n", probe_queue.size());
	}
#endif

#if 0
	{ std::string input = DPContainer::dump_objects(); std::string command = "dot -Tpng | display -resize 50% - &"; execute_command2 (command, input); }
#else
	Glib::RefPtr<Gtk::Application> kit = Gtk::Application::create(argc, argv, "org.flatcap.dparted");

	DParted d;
	d.set_data (&disks);

	kit->run (d);
#endif

	}
	DPContainer::dump_leaks();
	//log_info ("done\n"); //XXX move to log_close
	log_close();
	return 0;
}

