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


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>

#include <queue>
#include <string>
#include <vector>

#include "container.h"
#include "disk.h"
#include "filesystem.h"
#include "loop.h"
#include "table.h"
#include "volumegroup.h"

#include "utils.h"

std::queue<Container*> probe_queue;

/**
 * queue_add_probe
 */
void queue_add_probe (Container *item)
{
	if (!item)
		return;

	probe_queue.push (item);
	std::string s = get_size (item->parent_offset);
	//fprintf (stderr, "QUEUE: %s %s - %lld (%s)\n", item->name.c_str(), item->device.c_str(), item->parent_offset, s.c_str());
	//printf ("QUEUE has %lu items\n", probe_queue.size());
}

/**
 * mounts_get_list
 */
unsigned int mounts_get_list (Container &mounts)
{
	std::string command;
	std::string output;
	std::string error;
	std::vector<std::string> lines;
	unsigned int i;

	command = "grep '^/dev' /proc/mounts";
	execute_command (command, output, error);
	explode ("\n", output, lines);

	for (i = 0; i < lines.size(); i++) {
		std::string line = lines[i];
		printf ("line%d:\n%s\n\n", i, line.c_str());
	}

	return mounts.children.size();
}

/**
 * main
 */
int main (int argc, char *argv[])
{
	Container disks;
	Container *item = NULL;

	disks.name = "container";	//XXX dummy

	//Disk::find_devices (disks);
	Loop::find_devices (disks);
	//VolumeGroup::find_devices (disks);

	unsigned char *buffer = NULL;
	int bufsize = 4096;
	unsigned int j;

	for (j = 0; j < disks.children.size(); j++) {
		queue_add_probe (disks.children[j]);
	}

	buffer = (unsigned char*) malloc (bufsize);
	if (!buffer)
		return 1;

	while ((item = probe_queue.front())) {
#if 0
		fprintf (stderr, "queued item: '%s'\n", item->name.c_str());
#endif
		probe_queue.pop();
		//printf ("QUEUE has %lu items\n", probe_queue.size());

#if 0
		std::string s1;
		std::string s2;
		s1 = get_size (item->parent_offset);
		s2 = get_size (item->bytes_size);
		fprintf (stderr, "\tdevice     = %s\n",        item->device.c_str());
		fprintf (stderr, "\toffset     = %lld (%s)\n", item->parent_offset, s1.c_str());
		fprintf (stderr, "\ttotal size = %lld (%s)\n", item->bytes_size, s2.c_str());
		fprintf (stderr, "\n");
#endif
		item->read_data (0, bufsize, buffer);

#if 1
		Filesystem *f = Filesystem::probe (item, buffer, bufsize);
		if (f) {
			//printf ("\tfilesystem: %s\n", f->name.c_str());
			//delete f;
			continue;
		}
#endif

#if 1
		Table *t = Table::probe (item, buffer, bufsize);
		if (t) {
			//printf ("\ttable: %s\n", t->name.c_str());
			//printf ("\t\tuuid = %s\n", t->uuid.c_str());
			//delete t;
			continue;
		}
#endif

		//logicals...

		//empty
	}

#if 1
	std::string dot;
	dot += "digraph disks {\n";
	dot += "graph [ rankdir = \"TB\", bgcolor = grey ];\n";
	dot += "node [ shape = record, color = black, fillcolor = lightcyan, style = filled ];\n";

	unsigned int i;
	for (i = 0; i < disks.children.size(); i++) {
		dot += disks.children[i]->dump_dot();
	}
	dot += "\n};";

	printf ("%s\n", dot.c_str());
#endif

	return 0;
}

