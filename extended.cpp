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

#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>

#include <sstream>
#include <string>

#include "extended.h"
#include "log.h"
#include "main.h"
#include "misc.h"
#include "partition.h"
#include "utils.h"
#include "log_trace.h"

/**
 * Extended
 */
Extended::Extended (void)
{
	declare ("extended");
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
Extended *
Extended::probe (DPContainer &top_level, DPContainer *parent, long offset, long size)
{
	//LOG_TRACE;
	Extended *ext = nullptr;

	unsigned char *buffer = nullptr;
	int bufsize = 512;
	//off_t seek = 0;
	//ssize_t count = 0;
	long table_offset = offset;

	// create extended
	// add to parent
	// create partitions
	// add to extended

	ext = new Extended();

	ext->name = "extended";
	ext->bytes_size = size;
	ext->device = parent->device;
	ext->parent_offset = offset;

	buffer = (unsigned char*) malloc (bufsize);
	if (!buffer)
		return nullptr;

	Misc *res1 = new Misc();
	res1->name          = "reserved";
	res1->bytes_size    = 512;		//align (512, 1024*1024);
	res1->bytes_used    = res1->bytes_size;
	res1->parent_offset = 0;					// Start of the partition
	ext->add_child (res1);		// change to add_reserved?

	for (int loop = 0; loop < 50; loop++) {		//what's the upper limit? prob 255 in the kernel
		std::string s = get_size (table_offset);
		//log_debug ("table_offset = %lld (%s)\n", table_offset, s.c_str());
		/*FILE *f =*/ parent->open_device();
		/*int r =*/ parent->read_data (table_offset, bufsize, buffer);
		//log_debug ("f = %p, r = %d\n", f, r);

		//dump_hex (buffer, bufsize);

		if (*(unsigned short int *) (buffer+510) != 0xAA55) {
			log_error ("not an extended partition\n");
			//log_debug ("%s (%s), %lld\n", parent->name.c_str(), parent->device.c_str(), parent->parent_offset);
			return nullptr;
		}

		//log_debug ("extended partition\n");

		int num = 0;
		std::vector<struct partition> vp;
		num = ext->read_table (buffer, bufsize, 0, vp);
		//log_debug ("num = %d\n", num);
		//dump_hex (buffer, bufsize);

		if ((num < 0) || (vp.size() > 2)) {
			log_error ("partition table is corrupt\n");	// bugger
			return nullptr;
		}

		for (auto &part : vp) {
#if 0
			if ((part.type != 5) || (part.type != 5)) {
				std::string s1 = get_size (part.start);
				std::string s2 = get_size (part.size);

				log_debug ("\tpartition (0x%02x)\n", part.type);
				log_debug ("\t\tstart = %lld (%s)\n", part.start, s1.c_str());
				log_debug ("\t\tsize  = %lld (%s)\n", part.size,  s2.c_str());
				log_debug ("\n");
			}
#endif
			DPContainer *c = nullptr;

			if ((part.type == 0x05) || (part.type == 0x0F)) {
				table_offset = offset + part.start;
			} else {
				c = new Partition();
				c->name = "partition";
				c->bytes_size = part.size;

				//c->parent_offset = table_offset + part.start;
				//c->device = parent->device;

				c->parent_offset = table_offset + part.start - ext->parent_offset;

				std::ostringstream part_name;
				part_name << parent->device;
				char last = parent->device[parent->device.length()-1];
				if (isdigit (last)) {
					part_name << 'p';
				}
				part_name << (loop+5);

				c->device = part_name.str();

				ext->add_child (c);
				queue_add_probe (c);
			}
		}
		if (vp.size() == 1)
			break;
	}

	ext->fill_space();		// optional

	free (buffer);
	return ext;
}


