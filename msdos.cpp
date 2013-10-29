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
#include <fcntl.h>
#include <linux/fs.h>
#include <linux/hdreg.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <sstream>
#include <string>

#include "extended.h"
#include "log.h"
#include "main.h"
#include "misc.h"
#include "msdos.h"
#include "partition.h"
#include "utils.h"
#include "log_trace.h"

/**
 * Msdos
 */
Msdos::Msdos (void)
{
	declare ("msdos");
}

/**
 * ~Msdos
 */
Msdos::~Msdos()
{
}


/**
 * read_chs
 */
void
Msdos::read_chs (unsigned char *buffer, int *cylinder, int *head, int *sector)
{
	if (!buffer || !cylinder || !head || !sector)
		return;

	*head     = buffer[0];
	*sector   = buffer[1] & 0x3F;
	*cylinder = buffer[2] + ((buffer[1] & 0xC0) << 2);
}

/**
 * read_partition
 */
bool
Msdos::read_partition (unsigned char *buffer, int index, struct partition *part)
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

	part->start = *(int *) (buffer + index + 8);
	part->start *= 512;

	part->size = *(int *) (buffer + index + 12);
	part->size *= 512;

	return true;
}

/**
 * read_table
 */
unsigned int
Msdos::read_table (unsigned char *buffer, int bufsize, long offset, std::vector<struct partition> &vp)
{
	struct partition part;

	for (int i = 0; i < 4; i++) {
		if (!read_partition (buffer, i, &part))	// could flag Msdos object as invalid
			continue;		// RAR or return -1

		vp.push_back (part);
	}

	return vp.size();
}


/**
 * probe
 */
DPContainer *
Msdos::probe (DPContainer &top_level, DPContainer *parent, unsigned char *buffer, int bufsize)
{
	//LOG_TRACE;
	Msdos *m = nullptr;
	int count = 0;

	if (*(unsigned short int *) (buffer+510) != 0xAA55)
		return nullptr;

	// and some other quick checks

	m = new Msdos();

	m->name = "msdos";
	m->bytes_size = parent->bytes_size;
	m->device = parent->device;
	m->parent_offset = 0;

	parent->add_child (m);	//RAR new

	std::vector<struct partition> vp;
	count = m->read_table (buffer, bufsize, 0, vp);

	if ((count < 0) || (vp.size() > 4)) {
		log_debug ("partition table is corrupt\n");	// bugger
		m->unref();
		return nullptr;
	}

	Misc *res1 = new Misc();
	res1->name          = "reserved";
	res1->bytes_size    = 512;		//align (512, 1024*1024);
	res1->bytes_used    = res1->bytes_size;
	res1->parent_offset = 0;					// Start of the partition
	m->add_child (res1);		// change to add_reserved?

	for (unsigned int i = 0; i < vp.size(); i++) {
#if 0
		std::string s1 = get_size (vp[i].start);
		std::string s2 = get_size (vp[i].size);

		log_debug ("partition %d (0x%02x)\n", i+1, vp[i].type);
		log_debug ("\tstart = %lld (%s)\n", vp[i].start, s1.c_str());
		log_debug ("\tsize  = %lld (%s)\n", vp[i].size,  s2.c_str());
		log_debug ("\n");
#endif
		DPContainer *c = nullptr;

		std::ostringstream part_name;
		part_name << m->device;
		char last = m->device[m->device.length()-1];
		if (isdigit (last)) {
			part_name << 'p';
		}
		part_name << (i+1);

		if ((vp[i].type == 0x05) || (vp[i].type == 0x0F)) {
			//log_debug ("vp[i].start = %lld\n", vp[i].start);
			c = Extended::probe (top_level, m, vp[i].start, vp[i].size);
			if (!c)
				continue;

			c->parent_offset = vp[i].start;
			c->device = part_name.str();
		} else {
			Partition *p = new Partition();
			p->ptype = vp[i].type;
			c = p;
			c->name = "partition";
			c->bytes_size = vp[i].size;

			c->parent_offset = vp[i].start;
			c->device = part_name.str();

			queue_add_probe (c);
		}
		m->add_child (c);
	}

	m->fill_space();		// optional

	return m;
}


