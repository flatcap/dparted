/* Copyright (c) 2011 Richard Russon (FlatCap)
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

#include "msdos.h"
#include "utils.h"

/**
 * Msdos
 */
Msdos::Msdos (void)
{
	type = "\e[34mpart.table\e[0m";
}

/**
 * ~Msdos
 */
Msdos::~Msdos()
{
}


/**
 * dump
 */
void Msdos::dump (int indent /* = 0 */)
{
	std::string size = get_size (bytes_size);

	iprintf (indent, "\e[01;34mmsdos\e[0m (%s)\n", size.c_str());
	Container::dump (indent);
}

/**
 * dump_csv
 */
void Msdos::dump_csv (void)
{
	printf ("%s,%s,%s,%ld,%s,%s,%lld,%lld,%lld\n",
		"MSDOS",
		"<none>",
		name.c_str(),
		block_size,
		label.c_str(),
		uuid.c_str(),
		bytes_size,
		bytes_used,
		bytes_size - bytes_used);
	Container::dump_csv();
}

