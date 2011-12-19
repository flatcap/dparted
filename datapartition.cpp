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

#include "datapartition.h"
#include "utils.h"

/**
 * DataPartition
 */
DataPartition::DataPartition (void) :
	num (0)
{
	type = "datapartition";
}

/**
 * ~DataPartition
 */
DataPartition::~DataPartition()
{
}


/**
 * dump
 */
void DataPartition::dump (int indent /* = 0 */)
{
	if (num < 0)
		return;

	std::string size = get_size (bytes_size);

	iprintf (indent,   "%s%d (%s)\n", device.c_str()+5, num, size.c_str());
	//iprintf (indent+8, "Type:   %s\n",   type.c_str());

	//iprintf (indent+8, "Number: %d\n",   num);
	//iprintf (indent+8, "Length: %s\n",   size.c_str());

	Partition::dump (indent);
}

/**
 * dump_csv
 */
void DataPartition::dump_csv (void)
{
	printf ("%s,%s,%s,%ld,%s,%lld,%lld,%lld\n",
		"DataPartition",
		device.c_str(),
		name.c_str(),
		block_size,
		uuid.c_str(),
		bytes_size,
		bytes_used,
		bytes_size - bytes_used);
	Partition::dump_csv();
}

/**
 * dump_dot
 */
std::string DataPartition::dump_dot (void)
{
	std::ostringstream output;

	output << dump_table_header ("DataPartition", "green");

	output << Partition::dump_dot();

	output << dump_table_footer();
	output << dump_dot_children();

	return output.str();
}
