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

#include "volume.h"
#include "utils.h"
#include "log.h"

/**
 * Volume
 */
Volume::Volume (void) :
	kernel_major (0),
	kernel_minor (0)
{
	type.push_back ("volume");
}

/**
 * ~Volume
 */
Volume::~Volume()
{
}


/**
 * dump_dot
 */
std::string Volume::dump_dot (void)
{
	std::ostringstream output;

	output << dump_table_header ("Volume", "purple");

	output << Whole::dump_dot();

	output << dump_row ("lv_attr",      lv_attr);
	output << dump_row ("kernel_major", kernel_major);
	output << dump_row ("kernel_minor", kernel_minor);

	output << dump_table_footer();
	output << dump_dot_children();

	return output.str();
}


/**
 * read_data
 */
int Volume::read_data (long long offset, long long size, unsigned char *buffer)
{
	//log_error ("%s\n", __PRETTY_FUNCTION__);
	//log_error ("%s - %s\n", name.c_str(), device.c_str());
	if (fd < 0) {
		open_device();
	}

	return Whole::read_data (offset, size, buffer);
}

