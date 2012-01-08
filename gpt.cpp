/* Copyright (c) 2011-2012 Richard Russon (FlatCap)
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


Gpt * Gpt::probe (unsigned char *buffer, int bufsize)
{
	Gpt *g = NULL;

	if (strncmp ((char*) buffer+512, "EFI PART", 8))	// XXX replace with strict identify function (static)
		return NULL;

	g = new Gpt;

	g->name = "gpt";
	g->bytes_size = 1234;		// unknown as yet
	g->bytes_used = 0;
	g->device = "unknown";
	g->device_offset = 0;
	g->block_size = 0;
	g->uuid = read_uuid (buffer+568);

	int i;
	int j;
	Partition *p = NULL;
	buffer += 1024;

	for (i = 0; i < 128; i++, buffer += 128) {
		if (*(long long*) (buffer+32) == 0)
			break;

		p = new Partition;
		p->bytes_size = 1234;
		p->bytes_used = 0;
		p->device = "unknown";
		p->device_offset = 0;
		p->uuid = read_uuid (buffer+16);
		//p->part_type_uuid = read_guid (buffer+0);

		if (buffer[56]) {
			for (j = 0; j < 32; j += 2) {
				if (buffer[56+j] == 0)
					break;
				p->name += buffer[56+j];
			}
		}

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
