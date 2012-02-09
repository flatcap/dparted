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
#include <fcntl.h>
#include <linux/fs.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/hdreg.h>

#include <string>
#include <sstream>

#include "msdos.h"
#include "utils.h"
#include "extended.h"
#include "partition.h"
#include "main.h"

/**
 * Msdos
 */
Msdos::Msdos (void)
{
	type = "msdos";
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
void Msdos::read_chs (unsigned char *buffer, int *cylinder, int *head, int *sector)
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
bool Msdos::read_partition (unsigned char *buffer, int index, struct partition *part)
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
unsigned int Msdos::read_table (unsigned char *buffer, int bufsize, int fd, long long offset, std::vector<struct partition> &vp)
{
	struct partition part;
	int i;

	for (i = 0; i < 4; i++) {
		if (!read_partition (buffer, i, &part))	// could flag Msdos object as invalid
			continue;		// RAR or return -1

		vp.push_back (part);
	}

	return vp.size();
}


/**
 * probe
 */
Msdos * Msdos::probe (Container *parent, unsigned char *buffer, int bufsize)
{
	Msdos *m = NULL;
	int fd;
	unsigned int i;
	int count = 0;

	if (*(unsigned short int *) (buffer+510) != 0xAA55)
		return NULL;

	// and some other quick checks

	m = new Msdos;

	m->name = "msdos";
	m->bytes_size = 0;
	m->device = parent->device;
	m->device_offset = 0;

	parent->add_child (m);

	fd = open (parent->device.c_str(), O_RDONLY);
#if 0
	struct hd_geometry geometry;

	ioctl(fd, HDIO_GETGEO, &geometry);
	printf ("heads     = %d\n", geometry.heads);
	printf ("sectors   = %d\n", geometry.sectors);
	printf ("cylinders = %d\n", geometry.cylinders);	// truncated at ~500GiB
#endif
	std::vector<struct partition> vp;
	count = m->read_table (buffer, bufsize, fd, 0, vp);

	if ((count < 0) || (vp.size() > 4)) {
		printf ("partition table is corrupt\n");	// bugger
		return NULL;
	}

	for (i = 0; i < vp.size(); i++) {
#if 1
		std::string s1 = get_size (vp[i].start);
		std::string s2 = get_size (vp[i].size);

		printf ("partition %d (0x%02x)\n", i+1, vp[i].type);
		printf ("\tstart = %lld (%s)\n", vp[i].start, s1.c_str());
		printf ("\tsize  = %lld (%s)\n", vp[i].size,  s2.c_str());
		printf ("\n");
#endif
		Container *c = NULL;

		if (vp[i].type == 0x05) {
			c = Extended::probe (m, fd, vp[i].start, vp[i].size);
			if (!c)
				continue;
		} else {
			c = new Partition;
			c->name = "partition";
			c->bytes_size = vp[i].size;
			c->device_offset = vp[i].start;
			//queue_add_probe (c);
		}
		m->add_child (c);
	}

	close (fd);	// XXX or keep it for later?
	return m;
}


/**
 * dump
 */
void Msdos::dump (int indent /* = 0 */)
{
	std::string size = get_size (bytes_size);

	iprintf (indent, "msdos (%s)\n", size.c_str());
	Table::dump (indent);
}

/**
 * dump_csv
 */
void Msdos::dump_csv (void)
{
	printf ("%s,%s,%s,%ld,%s,%lld,%lld,%lld\n",
		"MSDOS",
		"<none>",
		name.c_str(),
		block_size,
		uuid.c_str(),
		bytes_size,
		bytes_used,
		bytes_size - bytes_used);
	Table::dump_csv();
}

/**
 * dump_dot
 */
std::string Msdos::dump_dot (void)
{
	std::ostringstream output;

	output << dump_table_header ("Msdos", "yellow");

	// no specfics for now

	output << Table::dump_dot();

	output << dump_table_footer();
	output << dump_dot_children();

	return output.str();
}

