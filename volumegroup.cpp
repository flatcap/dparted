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

#include "volumegroup.h"
#include "utils.h"

/**
 * VolumeGroup
 */
VolumeGroup::VolumeGroup (void) :
	pv_count (0),
	lv_count (0),
	vg_extent_count (0),
	vg_free_count (0),
	vg_seqno (0)
{
	type = "volumegroup";
}

/**
 * ~VolumeGroup
 */
VolumeGroup::~VolumeGroup()
{
}


/**
 * probe
 */
bool VolumeGroup::probe (const std::string &name, int fd, struct stat &st, Container &list)
{
	return false;
}

/**
 * find_devices
 */
unsigned int VolumeGroup::find_devices (Container &list)
{
	int retval = -1;

	// LVM2_VG_NAME=tmpvol
	// LVM2_PV_COUNT=3
	// LVM2_LV_COUNT=8
	// LVM2_VG_ATTR=wz--n-
	// LVM2_VG_SIZE=3208642560
	// LVM2_VG_FREE=104857600
	// LVM2_VG_UUID=G95j9O-fOkt-zdbk-wsxY-jupm-8MR6-TpZllQ
	// LVM2_VG_EXTENT_SIZE=4194304
	// LVM2_VG_EXTENT_COUNT=765
	// LVM2_VG_FREE_COUNT=25
	// LVM2_VG_SEQNO=13
	// LVM2_PV_NAME=/dev/loop0

	std::string command = "vgs --unquoted --separator='\t' --units=b --nosuffix --nameprefixes --noheadings --options vg_name,pv_count,lv_count,vg_attr,vg_size,vg_free,vg_uuid,vg_extent_size,vg_extent_count,vg_free_count,vg_seqno,pv_name";
	std::string output;
	std::string error;

	retval = execute_command (command, output, error);
	if (retval < 0)
		return 0;

	//printf ("%s\n", output.c_str());

	std::string name;
	std::string comp;
	unsigned int count;
	std::vector<std::string> lines;
	unsigned int i;
	std::map<std::string,StringNum> tags;

	count = explode ("\n", output, lines);
	//printf ("%d lines\n", count);

	for (i = 0; i < count; i++) {
		parse_tagged_line ((lines[i]), tags);

		name = tags["LVM2_VG_NAME"];
		comp = tags["LVM2_PV_NAME"];
		VolumeGroup *vg = static_cast<VolumeGroup*> (list.find_name (name));
		if (vg) {
			vg->components.push_back (comp);
		} else {
			vg = new VolumeGroup;
			vg->name = name;

			vg->pv_count   = tags["LVM2_PV_COUNT"];
			vg->lv_count   = tags["LVM2_LV_COUNT"];
			vg->vg_attr    = tags["LVM2_VG_ATTR"];
			vg->uuid       = tags["LVM2_VG_UUID"];
			vg->block_size = tags["LVM2_VG_EXTENT_SIZE"];
			vg->vg_seqno   = tags["LVM2_VG_SEQNO"];
			vg->bytes_size = tags["LVM2_VG_SIZE"];

			// LVM2_VG_FREE=104857600
			// LVM2_VG_EXTENT_COUNT=765
			// LVM2_VG_FREE_COUNT=25

			vg->components.push_back (comp);
			list.add_child (vg);
		}
	}

	//printf ("%lu objects\n", list.children.size());
	return list.children.size();
}

/**
 * dump
 */
void VolumeGroup::dump (int indent /* = 0 */)
{
	std::string size = get_size (bytes_size);
	std::string free = get_size (bytes_size - bytes_used);

	iprintf (indent, "%s (%s)\n", name.c_str(), size.c_str());

	iprintf (indent+8, "pv_count:        %ld\n", pv_count);
	iprintf (indent+8, "vg_free:         %s\n", free.c_str());
	iprintf (indent+8, "vg_uuid:         %s\n", uuid.c_str());

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
	output << dump_row ("vg_extent_count", vg_extent_count);
	output << dump_row ("vg_free_count",   vg_free_count);
	output << dump_row ("vg_seqno",        vg_seqno);

	output << dump_table_footer();
	output << dump_dot_children();

	return output.str();
}

