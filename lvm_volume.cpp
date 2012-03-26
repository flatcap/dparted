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


#include "lvm_volume.h"

#include <sstream>

/**
 * LVMVolume
 */
LVMVolume::LVMVolume (void)
{
	type.push_back ("lvm_volume");
}

/**
 * ~LVMVolume
 */
LVMVolume::~LVMVolume()
{
}


/**
 * dump_dot
 */
std::string LVMVolume::dump_dot (void)
{
	std::ostringstream output;

	output << dump_table_header ("LVMVolume", "pink");

	// no specfics for now

	//output << Volume::dump_dot(); //RAR skip a generation
	output << Whole::dump_dot();

	output << dump_table_footer();
	output << dump_dot_children();

	return output.str();
}


/**
 * read_data
 */
int LVMVolume::read_data (long long offset, long long size, unsigned char *buffer)
{
	//log_error ("%s\n", __PRETTY_FUNCTION__);
	//log_error ("%s - %s\n", name.c_str(), device.c_str());
	if (fd < 0) {
		open_device();
	}

	return Whole::read_data (offset, size, buffer);
}

