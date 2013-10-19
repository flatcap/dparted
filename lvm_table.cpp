/* Copyright (c) 2013 Richard Russon (FlatCap)
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
#include <cstring>

#include "log.h"
#include "lvm_table.h"
#include "utils.h"
#include "log_trace.h"
#include "main.h"
#include "lvm_group.h"
#include "lvm_partition.h"

#include "utils.h"

/**
 * LVMTable
 */
LVMTable::LVMTable (void)
{
	declare ("lvm_table");
}

/**
 * ~LVMTable
 */
LVMTable::~LVMTable()
{
}


/**
 * read_uuid_string
 */
std::string read_uuid_string (unsigned char *buffer)
{
	char text[33];
	std::string dashes;

	memcpy (text, buffer, sizeof (text) - 1);
	text[sizeof (text) - 1] = 0;

	dashes = text;

	dashes = dashes.substr(0,6) + "-" +
		 dashes.substr(6,4) + "-" +
		 dashes.substr(10,4) + "-" +
		 dashes.substr(14,4) + "-" +
		 dashes.substr(18,4) + "-" +
		 dashes.substr(22,4) + "-" +
		 dashes.substr(26,6);
	return dashes;
}

/**
 * fd_pvs - Attach all the Segments (LVMGroup and Volumes)
 */
void fd_pvs (DPContainer *parent)
{
	//LOG_TRACE;

	std::string command;
	std::vector<std::string> output;
	std::string error;
	std::map<std::string,StringNum> tags;

	/* Output fields from the 'pvs' command:
         * dev_size          - LVM2_DEV_SIZE          - Size of underlying device in current units.
         * lv_name           - LVM2_LV_NAME           - Name.  LVs created for internal use are enclosed in brackets.
         * lv_uuid           - LVM2_LV_UUID           - Unique identifier.
         * pe_start          - LVM2_PE_START          - Offset to the start of data on the underlying device.
         * pvseg_size        - LVM2_PVSEG_SIZE        - Number of extents in segment.
         * pvseg_start       - LVM2_PVSEG_START       - Physical Extent number of start of segment.
         * pv_attr           - LVM2_PV_ATTR           - Various attributes - see man page.
         * pv_count          - LVM2_PV_COUNT          - Number of PVs.
         * pv_free           - LVM2_PV_FREE           - Total amount of unallocated space in current units.
         * pv_name           - LVM2_PV_NAME           - Name.
         * pv_pe_alloc_count - LVM2_PV_PE_ALLOC_COUNT - Total number of allocated Physical Extents.
         * pv_pe_count       - LVM2_PV_PE_COUNT       - Total number of Physical Extents.
         * pv_size           - LVM2_PV_SIZE           - Size of PV in current units.
         * pv_used           - LVM2_PV_USED           - Total amount of allocated space in current units.
         * pv_uuid           - LVM2_PV_UUID           - Unique identifier.
         * segtype           - LVM2_SEGTYPE           - Type of LV segment.
         * vg_extent_size    - LVM2_VG_EXTENT_SIZE    - Size of Physical Extents in current units.
         * vg_name           - LVM2_VG_NAME           - Name.
         * vg_seqno          - LVM2_VG_SEQNO          - Revision number of internal metadata.  Incremented whenever it changes.
         * vg_uuid           - LVM2_VG_UUID           - Unique identifier.
	 */

	command = "sudo pvs --unquoted --separator='\t' --units=b --nosuffix --nameprefixes --noheadings --options dev_size,pe_start,pvseg_size,pvseg_start,pv_attr,pv_count,pv_free,pv_name,pv_pe_alloc_count,pv_pe_count,pv_size,pv_used,pv_uuid,segtype,vg_extent_size,vg_name,vg_seqno,vg_uuid,lv_uuid,lv_name";
	//log_error ("device = %s\n", parent->get_device_name().c_str());
	command += " " + parent->get_device_name();
	execute_command (command, output);

	for (auto line : output) {
		//log_debug ("OUT: %s\n\n", line.c_str());
		parse_tagged_line (line, "\t", tags);

		std::string lv_type = tags["LVM2_SEGTYPE"];
		if (lv_type == "free")				//XXX could process this and add it to the VGSeg's empty list
			continue;

		LVMPartition *vol_seg = new LVMPartition;
		//log_debug ("new LVMPartition (%p)\n", vol_seg);

		vol_seg->dev_size	= tags["LVM2_DEV_SIZE"];
		vol_seg->pe_start	= tags["LVM2_PE_START"];
		vol_seg->pv_attr	= tags["LVM2_PV_ATTR"];
		vol_seg->pv_count	= tags["LVM2_PV_COUNT"];
		vol_seg->pv_free	= tags["LVM2_PV_FREE"];
		vol_seg->pv_name	= tags["LVM2_PV_NAME"];
		vol_seg->pv_pe_alloc	= tags["LVM2_PV_PE_ALLOC_COUNT"];
		vol_seg->pv_pe_count	= tags["LVM2_PV_PE_COUNT"];
		vol_seg->pv_size	= tags["LVM2_PV_SIZE"];
		vol_seg->pv_used	= tags["LVM2_PV_USED"];
		vol_seg->pv_uuid	= tags["LVM2_PV_UUID"];

		vol_seg->pvseg_size	= tags["LVM2_PVSEG_SIZE"];
		vol_seg->pvseg_start	= tags["LVM2_PVSEG_START"];

		vol_seg->lv_name	= tags["LVM2_LV_NAME"];
		vol_seg->lv_type	= tags["LVM2_SEGTYPE"];
		vol_seg->lv_uuid	= tags["LVM2_LV_UUID"];

		vol_seg->vg_extent	= tags["LVM2_VG_EXTENT_SIZE"];
		vol_seg->vg_name	= tags["LVM2_VG_NAME"];
		vol_seg->vg_seqno	= tags["LVM2_VG_SEQNO"];
		vol_seg->vg_uuid	= tags["LVM2_VG_UUID"];

		vol_seg->name = vol_seg->lv_name;	//XXX container members
		vol_seg->uuid = vol_seg->lv_uuid;

		vol_seg->bytes_size	 = vol_seg->pvseg_size;
		vol_seg->bytes_size	*= vol_seg->vg_extent;
		vol_seg->bytes_used	 = vol_seg->bytes_size;
		vol_seg->parent_offset	 = vol_seg->pvseg_start;
		vol_seg->parent_offset	*= vol_seg->vg_extent;

#if 0
		//std::string seg_id = vg_name + ":" + lv_name + ":" + pv_name + "(" + start + ")";
		std::string seg_id = pv_name + "(" + start + ")";
		//log_debug ("\tseg_id = %s\n", seg_id.c_str());

		//log_debug ("lookup uuid %s\n", vg_uuid.c_str());
		LVMGroup *vg = vg_lookup[vg_uuid];
		//log_debug ("vg extent size = %ld\n", vg->block_size);

		LVMTable *vg_seg = vg_seg_lookup[dev];	//XXX this should exist
		if (!vg_seg)
			continue;			//XXX error?
#endif
#if 1
		//vol_seg->type = lv_type;			// linear, striped, etc

		vol_seg->whole = NULL; //RAR we don't know this yet

		//log_debug ("whole = %p\n", vg_seg->whole);

		parent->add_child (vol_seg);

		//log_debug ("add_child %p [%s/%s] -> %p [%s/%s]\n", vg_seg, vg_seg->type.back().c_str(), vg_seg->name.c_str(), vol_seg, vol_seg->type.back().c_str(), vol_seg->name.c_str());
		//log_debug ("vol_seg_lookup: %s -> %s\n", seg_id.c_str(), vol_seg->name.c_str());
		//vol_seg_lookup[seg_id] = vol_seg;

		//std::string vol_id = pv_name + "(" + start + ")";	//XXX dupe
#endif
	}
	//log_debug ("\n");
}

/**
 * probe
 */
DPContainer * LVMTable::probe (DPContainer &top_level, DPContainer *parent, unsigned char *buffer, int bufsize)
{
	//LOG_TRACE;
	LVMTable *t = NULL;

	if (strncmp ((const char*) buffer+512, "LABELONE", 8))
		return NULL;

	if (strncmp ((const char*) buffer+536, "LVM2 001", 8))
		return NULL;

	t = new LVMTable();
	//log_debug ("new LVMTable (%p)\n", t);

	t->uuid = read_uuid_string (buffer+544);

	//log_info ("table: %s\n", t->uuid.c_str());

	const int bufsize2 = 4096;
	std::vector<unsigned char> buffer2 (bufsize2);

	parent->open_device();
	parent->read_data (4608, bufsize2, &buffer2[0]);	// check read num

	char *ptr = (char*) &buffer[0];
	//dump_hex2 (ptr, 0, 600);
	t->bytes_size = *(uint64_t*) (ptr+576);
	t->parent_offset = 0;		//XXX what about if we're in a partition?
	t->bytes_used = 1048576*4;	//XXX pv metadata	XXX plus any slack space at the end (3MiB) check pv_pe_count
	//printf ("size of %s = %lld\n", t->uuid.c_str(), t->bytes_size);

	//dump_hex2 (&buffer2[0], 0, bufsize2);

	auto iter = buffer2.begin();
	for (;; iter++) {
		if (*iter == ' ')
			break;
	}

	std::string v(buffer2.begin(), iter);
	//log_error ("vol name = '%s'\n", v.c_str());
	t->vol_name = v;
#if 0
	std::string config (buffer2.begin(), buffer2.end());
	printf ("config = \n%s\n", buffer2.data());
#endif

	parent->add_child (t);

	// discover children, either 'pvs' or parse lvm config in header
	fd_pvs (t);

	return t;
}


