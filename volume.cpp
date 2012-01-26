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

#include <string>
#include <sstream>

#include "volume.h"
#include "utils.h"

/**
 * Volume
 */
Volume::Volume (void) :
	kernel_major (0),
	kernel_minor (0)
{
	type = "volume";
}

/**
 * ~Volume
 */
Volume::~Volume()
{
}


/**
 * dump
 */
void Volume::dump (int indent /* = 0 */)
{
	std::string size = get_size (bytes_size);

	iprintf (indent,   "%s (%s)\n", name.c_str(), size.c_str());
	iprintf (indent+8, "path: %s\n", device.c_str());

	//iprintf (indent+8, "major: %d\n", kernel_major);
	//iprintf (indent+8, "minor: %d\n", kernel_minor);
	//iprintf (indent+8, "attr: %s\n", lv_attr.c_str());

#if 0
	for (unsigned int i = 0; i < segments.size(); i++) {
		std::string vo = get_size (segments[i].volume_offset);
		std::string ss = get_size (segments[i].segment_size);
		std::string dd = get_size (segments[i].device_offset);
		iprintf (indent+8,  "Segment %d\n", i);
		iprintf (indent+16, "vol offset    = %s\n", vo.c_str());
		iprintf (indent+16, "seg size      = %s\n", ss.c_str());
		iprintf (indent+16, "device        = %s (+%s)\n", segments[i].device.c_str(), dd.c_str());

		//iprintf (indent+16, "seg size      = %lld\n", segments[i].segment_size);
		//iprintf (indent+16, "vol offset    = %lld\n", segments[i].volume_offset);
		//iprintf (indent+16, "device offset = %lld\n", segments[i].device_offset);
	}
#endif

	Whole::dump (indent);
}

/**
 * dump_csv
 */
void Volume::dump_csv (void)
{
	Whole::dump_csv();
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

