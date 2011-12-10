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
#include <sstream>

#include "partition.h"
#include "utils.h"

/**
 * Partition
 */
Partition::Partition (void) :
	start (0),
	end (0),
	num (0)
{
	type = "\e[33mpartition\e[0m";
}

/**
 * ~Partition
 */
Partition::~Partition()
{
}


/**
 * dump
 */
void Partition::dump (int indent /* = 0 */)
{
	if (num < 0)
		return;

	std::string size = get_size (bytes_size);

	iprintf (indent,   "\e[33m%s%d\e[0m (%s)\n", device.c_str()+5, num, size.c_str());
	//iprintf (indent+8, "Type:   %s\n",   type.c_str());
	iprintf (indent+8, "Start:  %lld\n", start);

	//iprintf (indent+8, "Number: %d\n",   num);
	//iprintf (indent+8, "Length: %s\n",   size.c_str());
	//iprintf (indent+8, "End:    %lld\n", end);

	Container::dump (indent);
}

/**
 * dump_csv
 */
void Partition::dump_csv (void)
{
	printf ("%s,%s,%s,%ld,%s,%lld,%lld,%lld\n",
		"Partition",
		device.c_str(),
		name.c_str(),
		block_size,
		uuid.c_str(),
		bytes_size,
		bytes_used,
		bytes_size - bytes_used);
	Container::dump_csv();
}

/**
 * dump_dot
 */
std::string Partition::dump_dot (void)
{
	std::ostringstream output;

	output << dump_table_header ("Partition", "green");

	output << Container::dump_dot();

	output << dump_row ("start", start);
	output << dump_row ("end",   end);

	output << dump_table_footer();

#if 1
	// now iterate through all the children
	for (std::vector<Container*>::iterator i = children.begin(); i != children.end(); i++) {
		output << "\n";
		output << (*i)->dump_dot();
		output << "obj_" << this << " -> " << "obj_" << (*i) << ";\n";
		output << "\n";
	}
#endif

	return output.str();
}
