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


#include <sstream>

#include "lvm_partition.h"

/**
 * LVMPartition
 */
LVMPartition::LVMPartition (void)
{
	declare ("lvm_partition");
}

/**
 * ~LVMPartition
 */
LVMPartition::~LVMPartition()
{
}


/**
 * dump_dot
 */
std::string LVMPartition::dump_dot (void)
{
	std::ostringstream output;

	// no specifics for now

	output << Partition::dump_dot();

	return output.str();
}


