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
#include <sys/types.h>
#include <unistd.h>

#include <string>
#include <sstream>

#include "extended.h"
#include "partition.h"
#include "utils.h"
#include "main.h"
#include "log.h"

/**
 * Extended
 */
Extended::Extended (void)
{
	type.push_back ("extended");
}

/**
 * ~Extended
 */
Extended::~Extended()
{
}


/**
 * probe
 */
Extended * Extended::probe (DPContainer *parent, long long offset, long long size)
{
	Extended *ext = NULL;

	unsigned char *buffer = NULL;
	int bufsize = 512;
	//off_t seek = 0;
	//ssize_t count = 0;
	int loop = 0;
	long long table_offset = offset;

	// create extended
	// add to parent
	// create partitions
	// add to extended

	ext = new Extended;

	ext->name = "extended";
	ext->bytes_size = size;
	ext->device = parent->device;
	ext->parent_offset = offset;

	//parent->add_child (ext);

	buffer = (unsigned char*) malloc (bufsize);
	if (!buffer)
		return NULL;

	for (loop = 0; loop < 5; loop++) {
		//log_debug ("table_offset = %lld\n", table_offset);
		parent->read_data (table_offset, bufsize, buffer);

		if (*(unsigned short int *) (buffer+510) != 0xAA55) {
			log_debug ("not an extended partition\n");
			return NULL;
		}

		//log_debug ("extended partition\n");

		int num = 0;
		unsigned int i;
		std::vector<struct partition> vp;
		num = ext->read_table (buffer, bufsize, 0, vp);
		//log_debug ("num = %d\n", num);
		//dump_hex (buffer, bufsize);

		if ((num < 0) || (vp.size() > 2)) {
			log_error ("partition table is corrupt\n");	// bugger
			return NULL;
		}

		for (i = 0; i < vp.size(); i++) {
#if 0
			if (vp[i].type != 5) {
				std::string s1 = get_size (vp[i].start);
				std::string s2 = get_size (vp[i].size);

				log_debug ("\tpartition %d (0x%02x)\n", i+1, vp[i].type);
				log_debug ("\t\tstart = %lld (%s)\n", vp[i].start, s1.c_str());
				log_debug ("\t\tsize  = %lld (%s)\n", vp[i].size,  s2.c_str());
				log_debug ("\n");
			}
#endif
			DPContainer *c = NULL;

			if (vp[i].type == 0x05) {
				table_offset = offset + vp[i].start;
			} else {
				c = new Partition;
				c->name = "partition";
				c->bytes_size = vp[i].size;

				//c->parent_offset = table_offset + vp[i].start;
				//c->device = parent->device;

				char num = '5' + loop;
				c->parent_offset = table_offset + vp[i].start - ext->parent_offset;
				c->device = parent->device + num;

				ext->add_child (c);
				queue_add_probe (c);
			}
		}
		if (vp.size() == 1)
			break;
	}

	free (buffer);
	return ext;
}


/**
 * dump_dot
 */
std::string Extended::dump_dot (void)
{
	std::ostringstream output;

	output << dump_table_header ("Extended", "yellow");

	// no specfics for now

	output << DPContainer::dump_dot(); //RAR skip a level for now

	output << dump_table_footer();
	output << dump_dot_children();

	output << "{ rank=same obj_" << this << " obj_" << parent << " }\n";

	return output.str();
}
