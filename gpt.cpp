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


#include <cstring>
#include <sstream>

#include "gpt.h"
#include "log.h"
#include "main.h"
#include "misc.h"
#include "partition.h"
#include "utils.h"
#include "filesystem.h"

/**
 * Gpt
 */
Gpt::Gpt (void)
{
	declare ("gpt");
}

/**
 * ~Gpt
 */
Gpt::~Gpt()
{
}


/**
 * align
 */
long long align (long long num, long long round)
{
	if (round == 0)
		return num;
	return ((num + round - 1) / round * round);
}

/**
 * fill_space
 */
long long Gpt::fill_space (void)
{
	// iterate through the children and add a Misc for all the unallocated regions
	// the size of these fillers must be >= alignment size

	std::vector<DPContainer*> vm;
	std::vector<DPContainer*>::iterator i;
#if 0
	std::string s1;
	std::string s2;
	log_debug ("fill space\n");
#endif

	long long upto = 0;

	for (i = children.begin(); i != children.end(); i++) {
		DPContainer *c = (*i);

		if (upto == c->parent_offset) {
			upto += c->bytes_size;
#if 0
			s1 = get_size (c->parent_offset);
			s2 = get_size (c->parent_offset + c->bytes_size);
			log_debug ("\tpartition %12lld -> %12lld    %8s -> %8s\n", c->parent_offset, c->parent_offset + c->bytes_size, s1.c_str(), s2.c_str());
#endif
		} else {
#if 0
			s1 = get_size (upto);
			s2 = get_size (c->parent_offset);
			log_debug ("\tspace     %12lld -> %12lld    %8s -> %8s\n", upto, c->parent_offset, s1.c_str(), s2.c_str());
			s1 = get_size (c->parent_offset);
			s2 = get_size (c->parent_offset + c->bytes_size);
			log_debug ("\tpartition %12lld -> %12lld    %8s -> %8s\n", c->parent_offset, c->parent_offset + c->bytes_size, s1.c_str(), s2.c_str());
#endif
			Misc *m = new Misc();
			m->name = "unallocated";
			m->parent_offset = upto;
			m->bytes_size = (c->parent_offset - upto);
			m->bytes_used = 0;

			vm.push_back (m);

			upto = c->parent_offset + c->bytes_size;
		}
	}

#if 1
	for (i = vm.begin(); i != vm.end(); i++) {
		add_child (*i);			// add_free()
	}
#endif

#if 0
	log_debug ("\nrecap\n");
	for (i = children.begin(); i != children.end(); i++) {
		DPContainer *c = (*i);
		s1 = get_size (c->bytes_size);
		log_debug ("\t%-12s %12lld -> %12lld  %9s\n", c->name.c_str(), c->parent_offset, c->parent_offset + c->bytes_size, s1.c_str());
	}
#endif

	return 0;
}


/**
 * probe
 */
DPContainer * Gpt::probe (DPContainer *parent, unsigned char *buffer, int bufsize)
{
	Gpt *g = NULL;

	if (strncmp ((char*) buffer+512, "EFI PART", 8))	// XXX replace with strict identify function (static)
		return NULL;

	// LBA		Description
	// ---------------------------------
	// 0		proetective mbr
	// 1		primary gpt header
	// 2-33		128 gpt entries
	// ...
	// -33		128 gpt entries
	// -1		secondary gpt header

	g = new Gpt;

	g->name = "gpt";
	g->bytes_size = parent->bytes_size;
	g->bytes_used = 0;
	g->device = parent->device;
	g->parent_offset = 0;
	g->block_size = 0;
	g->uuid = read_uuid (buffer+568);
	g->open_device();

	// Assumption: 1MiB alignment (for now)
	// Should reserved bits be allocated after actual partitions?
	// Should we allow Misc to overlap real partitions?
	// Then once a non-aligned partition is deleted
	// it would be replaced by an aligned partition.

#if 1
	Misc *res1 = new Misc();
	res1->name          = "reserved";
	res1->bytes_size    = 512 * 34;		//align (512 * 34, 1024*1024);
	res1->bytes_used    = res1->bytes_size;
	res1->parent_offset = 0;					// Start of the partition
	g->add_child (res1);		// change to add_reserved?

	Misc *res2 = new Misc();
	res2->name          = "reserved";
	res2->bytes_size    = 512 * 33;		//align (512 * 33, 1024*1024);
	res2->bytes_used    = res2->bytes_size;
	res2->parent_offset = g->bytes_size - res2->bytes_size;		// End of the partition
	g->add_child (res2);
#endif

	//log_debug ("res1 = %lld\n", res1->bytes_size);
	//log_debug ("res2 = %lld\n", res2->bytes_size);

	//std::cout << g << "\n";

	int i;
	int j;
	Partition *p = NULL;
	buffer += 1024;	//bufsize -= 1024; for range checking
#if 0
	p = new Partition;
	p->bytes_size = g->bytes_size / 2;
	p->parent_offset = g->bytes_size / 4;
	g->add_child (p);
	Filesystem *fs = new Filesystem;
	fs->bytes_size = p->bytes_size;
	fs->parent_offset = 0;
	p->add_child (fs);
	return g;
#endif
	for (i = 0; i < 128; i++, buffer += 128) {
		if (*(long long*) (buffer+32) == 0)
			break;

		//log_debug ("new Partition %d\n", i);
		p = new Partition;
		p->bytes_used = 0;
		p->uuid = read_uuid (buffer+16);
		//p->part_type_uuid = read_guid (buffer+0);

		std::ostringstream part_name;
		part_name << g->device;
		char last = g->device[g->device.length()-1];
		if (isdigit (last)) {
			part_name << 'p';
		}
		part_name << (i+1);
		p->device = part_name.str();

		long long start  = *(long long*) (buffer+32);
		long long finish = *(long long*) (buffer+40);

		p->parent_offset = start * 512;
		p->bytes_size = (finish - start + 1) * 512;

		p->name.clear();
		if (buffer[56]) {
			for (j = 0; j < 32; j += 2) {
				if (buffer[56+j] == 0)
					break;
				p->name += buffer[56+j];	// put this in a "label" member
			}
		}

#if 0
		std::string s = get_size (p->bytes_size);
		log_debug ("\t\tlabel  = %s\n",   p->name.c_str());
		log_debug ("\t\t\tstart  = %lld\n", *(long long*) (buffer+32) * 512);
		log_debug ("\t\t\tfinish = %lld\n", *(long long*) (buffer+40) * 512);
		log_debug ("\t\t\tsize   = %lld (%s)\n", p->bytes_size, s.c_str());
#endif
		g->add_child (p);
		queue_add_probe (p);
	}

	g->fill_space();		// optional

	return g;
}


/**
 * dump_dot
 */
std::string Gpt::dump_dot (void)
{
	std::ostringstream output;

	output << Table::dump_dot();

	// no specifics for now

	return output.str();
}
