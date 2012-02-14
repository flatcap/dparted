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
#include <string.h>

#include <sstream>

#include "gpt.h"
#include "partition.h"
#include "utils.h"
#include "main.h"

/**
 * Gpt
 */
Gpt::Gpt (void)
{
	type = "gpt";
}

/**
 * ~Gpt
 */
Gpt::~Gpt()
{
}


/**
 * probe
 */
Gpt * Gpt::probe (Container *parent, unsigned char *buffer, int bufsize)
{
	Gpt *g = NULL;

	if (strncmp ((char*) buffer+512, "EFI PART", 8))	// XXX replace with strict identify function (static)
		return NULL;

	g = new Gpt;

	g->name = "gpt";
	g->bytes_size = parent->bytes_size;
	g->bytes_used = 0;
	g->device = parent->device;
	g->device_offset = 0;
	g->block_size = 0;
	g->uuid = read_uuid (buffer+568);

	parent->add_child (g);

	int i;
	int j;
	Partition *p = NULL;
	buffer += 1024;

	for (i = 0; i < 128; i++, buffer += 128) {
		if (*(long long*) (buffer+32) == 0)
			break;

		p = new Partition;
		p->bytes_used = 0;
		p->device = g->device;
		p->uuid = read_uuid (buffer+16);
		//p->part_type_uuid = read_guid (buffer+0);

		long long start  = *(long long*) (buffer+32);
		long long finish = *(long long*) (buffer+40);

		p->device_offset = start * 512;
		p->bytes_size = (finish - start + 1) * 512;

		if (buffer[56]) {
			for (j = 0; j < 32; j += 2) {
				if (buffer[56+j] == 0)
					break;
				p->name += buffer[56+j];
			}
		}

		std::string s = get_size (p->bytes_size);
		//printf ("\t\tlabel  = %s\n",   p->name.c_str());
		//printf ("\t\t\tstart  = %lld\n", *(long long*) (buffer+32) * 512);
		//printf ("\t\t\tfinish = %lld\n", *(long long*) (buffer+40) * 512);
		//printf ("\t\t\tsize   = %lld (%s)\n", p->bytes_size, s.c_str());

		g->add_child (p);
		queue_add_probe (p);
	}

	return g;
}


/**
 * dump_dot
 */
std::string Gpt::dump_dot (void)
{
	std::ostringstream output;

	output << dump_table_header ("Gpt", "yellow");

	// no specfics for now

	output << Table::dump_dot();

	output << dump_table_footer();
	output << dump_dot_children();

	return output.str();
}
