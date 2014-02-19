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

#include <ctype.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <linux/hdreg.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <sstream>
#include <string>

#include "app.h"
#include "extended.h"
#include "log.h"
#include "main.h"
#include "misc.h"
#include "msdos.h"
#include "partition.h"
#include "utils.h"
#include "log_trace.h"
#include "visitor.h"

Msdos::Msdos (void)
{
	const char* me = "Msdos";

	sub_type (me);
}

MsdosPtr
Msdos::create (void)
{
	MsdosPtr m (new Msdos());
	m->weak = m;

	return m;
}


bool
Msdos::accept (Visitor& v)
{
	MsdosPtr m = std::dynamic_pointer_cast<Msdos> (get_smart());
	if (!v.visit(m))
		return false;
	return visit_children(v);
}


void
Msdos::read_chs (unsigned char* buffer, int* cylinder, int* head, int* sector)
{
	if (!buffer || !cylinder || !head || !sector)
		return;

	*head     = buffer[0];
	*sector   = buffer[1] & 0x3F;
	*cylinder = buffer[2] + ((buffer[1] & 0xC0) << 2);
}

bool
Msdos::read_partition (unsigned char* buffer, int index, struct partition* part)
{
	//XXX include this in read_table?
	if (!buffer || !part)
		return false;
	if ((index < 0) || (index > 3))
		return false;

	buffer += 446;
	index  *= 16;

	if (buffer[index + 4] == 0)
		return false;

	part->type = buffer[index + 4];

	part->start = *(int*) (buffer + index + 8);
	part->start *= 512;

	part->size = *(int*) (buffer + index + 12);
	part->size *= 512;

	return true;
}

unsigned int
Msdos::read_table (unsigned char* buffer, int UNUSED(bufsize), long UNUSED(offset), std::vector<struct partition>& vp)
{
	struct partition part;

	for (int i = 0; i < 4; i++) {
		if (!read_partition (buffer, i, &part))	// could flag Msdos object as invalid
			continue;		// RAR or return -1

		vp.push_back (part);
	}

	return vp.size();
}


ContainerPtr
Msdos::probe (ContainerPtr& top_level, ContainerPtr& parent, unsigned char* buffer, int bufsize)
{
	//LOG_TRACE;
	int count = 0;

	if (*(unsigned short int*) (buffer+510) != 0xAA55)
		return nullptr;

	// and some other quick checks

	MsdosPtr m = Msdos::create();

	m->name = "msdos";
	m->bytes_size = parent->bytes_size;
	//m->device = parent->device;	//XXX only for partitions, main body should inherit
	m->parent_offset = 0;

	ContainerPtr c(m);
	parent->add_child(c);	//RAR new

	std::vector<struct partition> vp;
	count = m->read_table (buffer, bufsize, 0, vp);

	if ((count < 0) || (vp.size() > 4)) {
		log_debug ("partition table is corrupt\n");	// bugger
		return nullptr;
	}

	MiscPtr res1 = Misc::create();
	res1->name          = "Reserved";
	res1->sub_type ("Reserved");
	res1->bytes_size    = 512;		//align (512, 1024*1024);
	res1->bytes_used    = res1->bytes_size;
	res1->parent_offset = 0;					// Start of the partition
	c = res1;
	m->add_child(c);		// change to add_reserved?

	for (unsigned int i = 0; i < vp.size(); i++) {
#if 0
		std::string s1 = get_size (vp[i].start);
		std::string s2 = get_size (vp[i].size);

		log_debug ("partition %d (0x%02x)\n", i+1, vp[i].type);
		log_debug ("\tstart = %lld (%s)\n", vp[i].start, s1.c_str());
		log_debug ("\tsize  = %lld (%s)\n", vp[i].size,  s2.c_str());
		log_debug ("\n");
#endif
		ContainerPtr c;

		std::ostringstream part_name;
		part_name << parent->device;
		char last = parent->device[parent->device.length()-1];
		if (isdigit (last)) {
			part_name << 'p';
		}
		part_name << (i+1);

		if ((vp[i].type == 0x05) || (vp[i].type == 0x0F)) {
			//log_debug ("vp[i].start = %lld\n", vp[i].start);
			ContainerPtr m2(m);
			c = Extended::probe (top_level, m2, vp[i].start, vp[i].size);
			if (!c)
				continue;

			c->parent_offset = vp[i].start;
			c->device = part_name.str();
		} else {
			PartitionPtr p = Partition::create();
			p->ptype = vp[i].type;
			c = p;
			c->name = "partition";
			c->bytes_size = vp[i].size;

			c->parent_offset = vp[i].start;
			c->device = part_name.str();

			main_app->queue_add_probe(c);
		}
		m->add_child(c);
	}

	m->fill_space();		// optional

	return m;
}


std::vector<Action>
Msdos::get_actions (void)
{
	// LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.msdos", true },
	};

	std::vector<Action> parent_actions = Table::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
Msdos::perform_action (Action action)
{
	if (action.name == "dummy.msdos") {
		std::cout << "Msdos perform: " << action.name << std::endl;
		return true;
	} else {
		return Table::perform_action (action);
	}
}


