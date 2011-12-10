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

#include "volumegroup.h"
#include "utils.h"

/**
 * VolumeGroup
 */
VolumeGroup::VolumeGroup (void) :
	pv_count (0),
	lv_count (0),
	vg_extent_size (0),
	vg_extent_count (0),
	vg_free_count (0),
	vg_seqno (0)
{
	type = "\e[35mvolumegroup\e[0m";
}

/**
 * ~VolumeGroup
 */
VolumeGroup::~VolumeGroup()
{
}


/**
 * dump
 */
void VolumeGroup::dump (int indent /* = 0 */)
{
	std::string size = get_size (bytes_size);
	std::string free = get_size (bytes_size - bytes_used);
	std::string extent = get_size (vg_extent_size);

	iprintf (indent, "\e[35m%s\e[0m (%s)\n", name.c_str(), size.c_str());

	iprintf (indent+8, "pv_count:        %d\n", pv_count);
	iprintf (indent+8, "vg_free:         %s\n", free.c_str());
	iprintf (indent+8, "vg_uuid:         %s\n", uuid.c_str());
	iprintf (indent+8, "vg_extent_size:  %s\n", extent.c_str());

	//iprintf (indent+8, "vg_attr:         %s\n",   vg_attr.c_str());
	//iprintf (indent+8, "vg_name:         %s\n",   vg_name.c_str());
	//iprintf (indent+8, "lv_count:        %d\n",   lv_count);
	//iprintf (indent+8, "vg_size:         %lld\n", vg_size);
	//iprintf (indent+8, "vg_extent_count: %lld\n", vg_extent_count);
	//iprintf (indent+8, "vg_free_count:   %lld\n", vg_free_count);
	//iprintf (indent+8, "vg_seqno:        %ld\n",  vg_seqno);

	Whole::dump (indent);
}

/**
 * dump_csv
 */
void VolumeGroup::dump_csv (void)
{
	Whole::dump_csv();
}

/**
 * dump_dot
 */
std::string VolumeGroup::dump_dot (void)
{
	std::ostringstream output;

	output << dump_table_header ("VolumeGroup", "#008080");

	output << Whole::dump_dot();

	output << dump_row ("pv_count",        pv_count);
	output << dump_row ("lv_count",        lv_count);
	output << dump_row ("vg_attr",         vg_attr);
	output << dump_row ("vg_extent_size",  vg_extent_size);
	output << dump_row ("vg_extent_count", vg_extent_count);
	output << dump_row ("vg_free_count",   vg_free_count);
	output << dump_row ("vg_seqno",        vg_seqno);

	output << dump_table_footer();

	// now iterate through all the children
	for (std::vector<Container*>::iterator i = children.begin(); i != children.end(); i++) {
		output << "\n";
		output << (*i)->dump_dot();
		output << "obj_" << this << " -> " << "obj_" << (*i) << ";\n";
		output << "\n";
	}

	return output.str();
}

