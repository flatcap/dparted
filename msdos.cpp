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
 * probe
 */
Msdos * Msdos::probe (Container *parent, unsigned char *buffer, int bufsize)
{
	Msdos *m = NULL;
	Partition *p = NULL;
	int i;

	if (*(unsigned short int *) (buffer+510) != 0xAA55)
		return NULL;

	m = new Msdos;

	m->name = "msdos";
	m->bytes_size = 0;
	m->device = parent->device;
	m->device_offset = 0;

	for (i = 0; i < 4; i++) {
		if (buffer[446 + (i*16) + 4] == 0)
			continue;
		p = new Partition;
		printf ("number %d : type 0x%02hhx\n", i+1, buffer[4]);

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

#if 0
		struct hd_geometry geometry;

		if (!ioctl(fd, HDIO_GETGEO, &geometry)) {
		kern_heads = geometry.heads;
		kern_sectors = geometry.sectors;
		/* never use geometry.cylinders - it is truncated */
#endif

		// number of heads = 255
		// number of sectors / track = 63
		lba = (c * 255 + h) * 63 + (s - 1);

		printf ("start\n");
		printf ("\tc   = %d\n", c);
		printf ("\th   = %d\n", h);
		printf ("\ts   = %d\n", s);
		printf ("\tlba = %d\n", lba);
		printf ("\tlba = %d\n", *(int *) (buffer + 446 + (i*16) + 8));

		h = buffer[446 + (i*16) + 5];
		s = buffer[446 + (i*16) + 6] & 0x3F;
		c = buffer[446 + (i*16) + 7] +
		    ((buffer[446 + (i*16) + 6] & 0xC0) << 2);

		// number of heads = 255
		// number of sectors / track = 63
		lba = (c * 255 + h) * 63 + (s - 1);

		printf ("end\n");
		printf ("\tc   = %d\n", c);
		printf ("\th   = %d\n", h);
		printf ("\ts   = %d\n", s);
		printf ("\tlba = %d\n", lba);
		printf ("\tsize= %d\n", *(int *) (buffer + 446 + (i*16) + 12));


		m->add_child (p);
		queue_add_probe (p);
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
