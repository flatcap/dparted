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


#include <string>
#include <sstream>

#include "partition.h"
#include "log.h"

/**
 * Partition
 */
Partition::Partition (void) :
	ptype (0)
{
	type.push_back ("partition");
}

/**
 * ~Partition
 */
Partition::~Partition()
{
}


/**
 * dump_dot
 */
std::string Partition::dump_dot (void)
{
	std::ostringstream output;

	output << dump_table_header ("Partition", "#d0dd80");

	// no specfics for now

	output << DPContainer::dump_dot();

	output << dump_table_footer();
	output << dump_dot_children();

	return output.str();
}
