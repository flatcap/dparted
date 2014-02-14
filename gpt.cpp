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

#include <cstring>
#include <sstream>

#include "app.h"
#include "gpt.h"
#include "log.h"
#include "main.h"
#include "misc.h"
#include "partition.h"
#include "utils.h"
#include "filesystem.h"
#include "log_trace.h"
#include "visitor.h"

/**
 * Gpt
 */
Gpt::Gpt (void)
{
	const char* me = "Gpt";

	sub_type (me);
}

/**
 * create
 */
GptPtr
Gpt::create (void)
{
	GptPtr g (new Gpt());
	g->weak = g;

	return g;
}


/**
 * accept
 */
bool
Gpt::accept (Visitor& v)
{
	GptPtr g = std::dynamic_pointer_cast<Gpt> (get_smart());
	if (!v.visit(g))
		return false;
	return visit_children(v);
}


/**
 * probe
 */
ContainerPtr
Gpt::probe (ContainerPtr& top_level, ContainerPtr& parent, unsigned char* buffer, int bufsize)
{
	//LOG_TRACE;

	if (strncmp ((char*) buffer+512, "EFI PART", 8))	// XXX replace with strict identify function (static)
		return nullptr;

	// LBA		Description
	// ---------------------------------
	// 0		protective mbr
	// 1		primary gpt header
	// 2-33		128 gpt entries
	// ...
	// -33		128 gpt entries
	// -1		secondary gpt header

	GptPtr g = create();

	g->name = "gpt";
	g->bytes_size = parent->bytes_size;
	g->bytes_used = 0;
	g->parent_offset = 0;
	g->block_size = 0;
	g->uuid = read_uuid1 (buffer+568);

	ContainerPtr c(g);
	parent->add_child(c); //RAR new

	// Assumption: 1MiB alignment (for now)
	// Should reserved bits be allocated after actual partitions?
	// Should we allow Misc to overlap real partitions?
	// Then once a non-aligned partition is deleted
	// it would be replaced by an aligned partition.

#if 0
	MiscPtr res1 = Misc::create();
	res1->name          = "reserved";
	res1->bytes_size    = 512 * 34;		//align (512 * 34, 1024*1024);
	res1->bytes_used    = res1->bytes_size;
	res1->parent_offset = 0;					// Start of the partition
	g->add_child (res1);		// change to add_reserved?

	MiscPtr res2 = Misc::create();
	res2->name          = "reserved";
	res2->bytes_size    = 512 * 33;		//align (512 * 33, 1024*1024);
	res2->bytes_used    = res2->bytes_size;
	res2->parent_offset = g->bytes_size - res2->bytes_size;		// End of the partition
	g->add_child (res2);
#endif

	//log_debug ("res1 = %lld\n", res1->bytes_size);
	//log_debug ("res2 = %lld\n", res2->bytes_size);

	//std::cout << g << "\n";

	buffer += 1024;	//bufsize -= 1024; for range checking
#if 0
	p = new Partition();
	p->bytes_size = g->bytes_size / 2;
	p->parent_offset = g->bytes_size / 4;
	g->add_child(p);
	FilesystemPtr fs = Filesystem::create();
	fs->bytes_size = p->bytes_size;
	fs->parent_offset = 0;
	p->add_child (fs);
	return g;
#endif
	for (int i = 0; i < 128; i++, buffer += 128) {
		if (*(long*) (buffer+32) == 0)
			continue;			// Skip empty slots

		//log_debug ("new Partition %d\n", i);
		PartitionPtr p = Partition::create();
		p->bytes_used = 0;
		p->uuid = read_uuid1 (buffer+16);
		//p->part_type_uuid = read_guid (buffer+0);

		std::ostringstream part_name;
		std::string device = g->get_device_name();
		part_name << device;
		if (isdigit (device.back())) {
			part_name << 'p';
		}
		part_name << (i+1);
		p->device = part_name.str();

		long start  = *(long*) (buffer+32);
		long finish = *(long*) (buffer+40);

		p->parent_offset = start * 512;
		p->bytes_size = (finish - start + 1) * 512;

		p->name.clear();
		if (buffer[56]) {
			for (int j = 0; j < 32; j += 2) {
				if (buffer[56+j] == 0)
					break;
				p->name += buffer[56+j];
			}
		}

#if 0
		std::string s = get_size (p->bytes_size);
		log_debug ("\t\tlabel  = %s\n",   p->name.c_str());
		log_debug ("\t\t\tstart  = %lld\n", *(long*) (buffer+32) * 512);
		log_debug ("\t\t\tfinish = %lld\n", *(long*) (buffer+40) * 512);
		log_debug ("\t\t\tsize   = %lld (%s)\n", p->bytes_size, s.c_str());
#endif
		ContainerPtr c(p);
		g->add_child(c);
		main_app->queue_add_probe(c);
	}

#if 0
	g->fill_space();		// optional
#endif

	return g;
}


/**
 * get_actions
 */
std::vector<Action>
Gpt::get_actions (void)
{
	// LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.gpt", true },
	};

	std::vector<Action> parent_actions = Table::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

/**
 * perform_action
 */
bool
Gpt::perform_action (Action action)
{
	if (action.name == "dummy.gpt") {
		std::cout << "Gpt perform: " << action.name << std::endl;
		return true;
	} else {
		return Table::perform_action (action);
	}
}


