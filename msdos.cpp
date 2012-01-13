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
 * read_table
 */
void Msdos::read_table (int fd, long long offset)
{
	unsigned char buffer[512];
	int i;

	lseek (fd, offset, SEEK_SET);
	read (fd, buffer, sizeof (buffer));

	for (i = 0; i < 4; i++) {
		if (buffer[446 + (i*16) + 4] == 0)
			continue;
		printf ("partition %d (0x%02x)\n", i+1, buffer[446 + (i*16) + 4]);

#if 0
		int c = 0;
		int h = 0;
		int s = 0;
		int lba = 0;

		h = buffer[446 + (i*16) + 1];
		s = buffer[446 + (i*16) + 2] & 0x3F;
		c = buffer[446 + (i*16) + 3] + ((buffer[446 + (i*16) + 2] & 0xC0) << 2);

		// number of heads = 255
		// number of sectors / track = 63
		lba = (c * 255 + h) * 63 + (s - 1);
#endif

#if 1
		//long long l1;
		long long l2;
		long long l3;

		//std::string s1;
		std::string s2;
		std::string s3;

		//l1 = (c * 255 + h) * 63 + (s - 1);
		l2 = *(int *) (buffer + 446 + (i*16) + 8);
		l2 *= 512;
		l3 = *(int *) (buffer + 446 + (i*16) + 12);
		l3 *= 512;

		//s1 = get_size (l1);
		s2 = get_size (l2);
		s3 = get_size (l3);

		//printf ("start\n");
		//printf ("\tchs = %d,%d,%d\n", c, h, s);
		//printf ("\tlba = %lld (%s)\n", l1, s1.c_str());
		printf ("\toffset = %lld (%s)\n", l2, s2.c_str());
		printf ("\tsize   = %lld (%s)\n", l3, s3.c_str());
#endif

#if 0
		h = buffer[446 + (i*16) + 5];
		s = buffer[446 + (i*16) + 6] & 0x3F;
		c = buffer[446 + (i*16) + 7] +
		    ((buffer[446 + (i*16) + 6] & 0xC0) << 2);

		// number of heads = 255
		// number of sectors / track = 63
		lba = (c * 255 + h) * 63 + (s - 1);
#endif

#if 0
		printf ("end\n");
		printf ("\tc   = %d\n", c);
		printf ("\th   = %d\n", h);
		printf ("\ts   = %d\n", s);
		printf ("\tlba = %d\n", lba);
		printf ("\tsize= %d\n", *(int *) (buffer + 446 + (i*16) + 12));
#endif
	}
}


/**
 * probe
 */
Msdos * Msdos::probe (Container *parent, unsigned char *buffer, int bufsize)
{
	Msdos *m = NULL;
	Partition *p = NULL;
	int i;
	int fd;

	if (*(unsigned short int *) (buffer+510) != 0xAA55)
		return NULL;

#if 1
	fd = open (parent->device.c_str(), O_RDONLY);
	read_table (fd, 0);		printf ("\n");
	read_table (fd, 75307679744);	printf ("\n");		// ext part: 75307679744 + 0
	read_table (fd, 117270642688);	printf ("\n");		//           75307679744 + 41962962944
	read_table (fd, 151841144832);	printf ("\n");		//           75307679744 + 76533465088
	close (fd);
	return NULL;
#endif

	m = new Msdos;

	m->name = "msdos";
	m->bytes_size = 0;
	m->device = parent->device;
	m->device_offset = 0;

#if 1
	struct hd_geometry geometry;

	fd = open (parent->device.c_str(), O_RDONLY);
	ioctl(fd, HDIO_GETGEO, &geometry);
	printf ("heads     = %d\n", geometry.heads);
	printf ("sectors   = %d\n", geometry.sectors);
	printf ("cylinders = %d\n", geometry.cylinders);	// truncated at ~500GiB
#endif

	for (i = 0; i < 4; i++) {
		if (buffer[446 + (i*16) + 4] == 0)
			continue;
		p = new Partition;
		printf ("number %d : type 0x%02x\n", i+1, buffer[446 + (i*16) + 4]);

		p->name = "partition";
		p->bytes_size = 1234;
		p->bytes_used = 0;
		p->device = m->device;
		p->device_offset = 1234;

		int c = 0;
		int h = 0;
		int s = 0;
		int lba = 0;

		h = buffer[446 + (i*16) + 1];
		s = buffer[446 + (i*16) + 2] & 0x3F;
		c = buffer[446 + (i*16) + 3] +
		    ((buffer[446 + (i*16) + 2] & 0xC0) << 2);

		// number of heads = 255
		// number of sectors / track = 63
		lba = (c * 255 + h) * 63 + (s - 1);

#if 1
		printf ("start\n");
		printf ("\tc   = %d\n", c);
		printf ("\th   = %d\n", h);
		printf ("\ts   = %d\n", s);
		printf ("\tlba = %d\n", lba);
		printf ("\tlba = %d\n", *(int *) (buffer + 446 + (i*16) + 8));
#endif

		h = buffer[446 + (i*16) + 5];
		s = buffer[446 + (i*16) + 6] & 0x3F;
		c = buffer[446 + (i*16) + 7] +
		    ((buffer[446 + (i*16) + 6] & 0xC0) << 2);

		// number of heads = 255
		// number of sectors / track = 63
		lba = (c * 255 + h) * 63 + (s - 1);

#if 0
		printf ("end\n");
		printf ("\tc   = %d\n", c);
		printf ("\th   = %d\n", h);
		printf ("\ts   = %d\n", s);
		printf ("\tlba = %d\n", lba);
		printf ("\tsize= %d\n", *(int *) (buffer + 446 + (i*16) + 12));
#endif

		m->add_child (p);
		//queue_add_probe (p);
	}

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
