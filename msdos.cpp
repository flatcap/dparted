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

struct partition {
	long long start;
	long long size;
	int type;
};

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
void Msdos::read_table (Container *parent, int fd, long long offset)
{
	unsigned char buffer[512];
	struct partition part;
	long long ext_start = -1;
	//long long ext_size = -1;
	int i;

	//printf ("SEEK %lld\n", offset);
	lseek (fd, offset, SEEK_SET);
	read (fd, buffer, sizeof (buffer));

	for (i = 0; i < 4; i++) {
		if (!read_partition (buffer, i, &part))
			continue;

		if (part.type == 0x05) {
			ext_start = part.start;
			//ext_size  = part.size;
			continue;
		}

		std::string s1 = get_size (part.start);
		std::string s2 = get_size (part.size);

#if 0
		printf ("partition %d (0x%02x)\n", i+1, part.type);
		printf ("\tstart = %lld (%s)\n", part.start, s1.c_str());
		printf ("\tsize  = %lld (%s)\n", part.size,  s2.c_str());
		printf ("\n");
#endif

		Partition *p = new Partition;
		p->bytes_size = part.size;
		p->device_offset = part.start;

		parent->add_child (p);
	}

	if (ext_start < 0)
		return;

	offset = ext_start;
	for (i = 5; i < 50; i++) {
		//printf ("SEEK %lld\n", offset);
		lseek (fd, offset, SEEK_SET);
		read (fd, buffer, sizeof (buffer));

		if (!read_partition (buffer, 0, &part))
			break;

		std::string s1 = get_size (part.start);
		std::string s2 = get_size (part.size);

#if 0
		printf ("partition %d (0x%02x)\n", i, part.type);
		printf ("\tstart = %lld (%s)\n", part.start, s1.c_str());
		printf ("\tsize  = %lld (%s)\n", part.size,  s2.c_str());
		printf ("\n");
#endif

		Extended *e = new Extended;
		e->bytes_size = part.size;
		e->device_offset = part.start;

		parent->add_child (e);

		if (!read_partition (buffer, 1, &part))
			break;

		if (part.type != 0x05) {
			break;
		}

		offset = ext_start + part.start;

	}
}


/**
 * probe
 */
Msdos * Msdos::probe (Container *parent, unsigned char *buffer, int bufsize)
{
	Msdos *m = NULL;
	int fd;

	if (*(unsigned short int *) (buffer+510) != 0xAA55)
		return NULL;

	m = new Msdos;

	m->name = "msdos";
	m->bytes_size = 0;
	m->device = parent->device;
	m->device_offset = 0;

	fd = open (parent->device.c_str(), O_RDONLY);
#if 0
	struct hd_geometry geometry;

	ioctl(fd, HDIO_GETGEO, &geometry);
	printf ("heads     = %d\n", geometry.heads);
	printf ("sectors   = %d\n", geometry.sectors);
	printf ("cylinders = %d\n", geometry.cylinders);	// truncated at ~500GiB
#endif
	read_table (m, fd, 0);

	close (fd);
	parent->add_child (m);
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

