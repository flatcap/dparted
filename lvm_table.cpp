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
#include "lvm2.h"

#include "utils.h"

/**
 * LvmTable
 */
LvmTable::LvmTable (void) :
	seq_num(0)
{
	declare ("lvm_table");
}

/**
 * ~LvmTable
 */
LvmTable::~LvmTable()
{
}


/**
 * read_uuid_string
 */
std::string read_uuid_string (char *buffer)
{
	if (!buffer)
		return "";

	std::string uuid ((char*) buffer, 32);

	uuid.insert (26, 1, '-');
	uuid.insert (22, 1, '-');
	uuid.insert (18, 1, '-');
	uuid.insert (14, 1, '-');
	uuid.insert (10, 1, '-');
	uuid.insert ( 6, 1, '-');

	return uuid;
}

/**
 * fd_pvs
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

		LvmPartition *vol_seg = new LvmPartition;
		//log_debug ("new LvmPartition (%p)\n", vol_seg);

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
		LvmGroup *vg = vg_lookup[vg_uuid];
		//log_debug ("vg extent size = %ld\n", vg->block_size);

		LvmTable *vg_seg = vg_seg_lookup[dev];	//XXX this should exist
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
 * get_label_header
 */
static struct label_header *
get_label_header (unsigned char *buffer)
{
	struct label_header *lh = (struct label_header *) buffer;

	if (!lh)
		return NULL;

	if (strncmp ((char*) lh->id, "LABELONE", 8))
		return NULL;

	if (strncmp ((char*) lh->type, "LVM2 001", 8))
		return NULL;

	//XXX validate crc, range check offset
	return lh;
}

/**
 * get_pv_header
 */
static struct pv_header *
get_pv_header (unsigned char *buffer)
{
	struct pv_header *ph = (struct pv_header *) buffer;

	if (!ph)
		return NULL;

	//XXX validate chars in uuid, disk_areas
	return ph;
}

/**
 * get_mda_header
 */
static struct mda_header *
get_mda_header (unsigned char *buffer)
{
	struct mda_header *mh = (struct mda_header *) buffer;

	if (!mh)
		return NULL;

	if (strncmp ((char*) mh->magic, FMTT_MAGIC, 16))
		return NULL;

	if (mh->version != FMTT_VERSION)
		return NULL;

	//XXX validate checksum
	return mh;
}

/**
 * get_seq_num
 */
static int
get_seq_num (const std::string &config)
{
	size_t index = config.find ("seqno = ");
	size_t end   = config.find ('\n', index);

	if (index == std::string::npos)
		return 0;

	//printf ("index = %ld, end = %ld, count = %ld\n", index, end, end-index-8);

	StringNum sn (config.substr (index+8, end-index-7));
	//printf ("num = %.5s\n", sn.c_str());

	return sn;
}

/**
 * get_vol_name
 */
static std::string
get_vol_name (const std::string &config)
{
	size_t end = config.find (" {\n");

	if (end == std::string::npos)
		return "";

	return config.substr (0, end);
}


#if 0
/**
 * format_config
 */
static void
format_config (std::string &config)
{
	size_t index = 0;
	size_t first;
	std::string indent;

	for (int i = 0; i < 1000; i++) {
		first = config.find_first_of ("[]{}\n", index);
		if (first == std::string::npos)
			break;
		//printf ("first = %lu '%c'\n", first, config[first] == '\n' ? '@' : config[first]);

		switch (config[first]) {
			case '[':
			case '{':
				indent += "\t";
				break;
			case ']':
			case '}':
				indent = indent.substr(1);
				if (config[first-1] == '\t') {
					config.erase(first-1, 1);
					first--;
				}
				break;
			case '\n':
				if (config[first+1] != '\n') {
					config.insert (first+1, indent);
					first += indent.size();
				}
				break;
		}

		index = first + 1;
	}
}

#endif
/**
 * probe
 */
DPContainer *
LvmTable::probe (DPContainer &top_level, DPContainer *parent, unsigned char *buffer, int bufsize)
{
	//LOG_TRACE;
	LvmTable *t = NULL;

	//XXX check bufsize gives us all the data we need

	struct label_header *lh = get_label_header (buffer+512);
	if (!lh)
		return NULL;

	//printf ("'%.8s', %lu, 0x%8x, %u, '%.8s'\n", lh->id, lh->sector_xl, lh->crc_xl, lh->offset_xl, lh->type);

	struct pv_header *ph = get_pv_header (buffer + 512 + lh->offset_xl);
	if (!ph)
		return NULL;

	std::string pv_uuid = read_uuid_string ((char*) ph->pv_uuid);

	//log_info ("%s, %lu (%s)\n", pv_uuid.c_str(), ph->device_size_xl, get_size (ph->device_size_xl).c_str());

#if 0
	printf ("Disk locations:\n");
	int i;
	for (i = 0; i < 8; i++) {
		if (ph->disk_areas_xl[i].offset == 0)
			break;
		printf ("\t%lu, %lu\n", ph->disk_areas_xl[i].offset, ph->disk_areas_xl[i].size);
	}
#endif

#if 0
	printf ("Metadata locations:\n");
	for (i++; i < 8; i++) {
		if (ph->disk_areas_xl[i].offset == 0)
			break;
		printf ("\t%lu, %lu (%lu)\n", ph->disk_areas_xl[i].offset, ph->disk_areas_xl[i].size, ph->disk_areas_xl[i].offset + ph->disk_areas_xl[i].size);
	}
#endif

	//XXX 4096 from metadata location
	struct mda_header *mh = get_mda_header (buffer + 4096);
	if (!mh)
		return NULL;

	//printf ("0x%08x, '%.16s', %u, %lu, %lu\n", mh->checksum_xl, mh->magic, mh->version, mh->start, mh->size);

#if 0
	printf ("Metadata:\n");
	for (i = 0; i < 4; i++) {
		if (mh->raw_locns[i].offset == 0)
			break;
		printf ("\t%lu (0x%lx), %lu, 0x%08x, %u\n", mh->raw_locns[i].offset, mh->raw_locns[i].offset, mh->raw_locns[i].size, mh->raw_locns[i].checksum, mh->raw_locns[i].flags);
	}
#endif

	int offset = mh->raw_locns[0].offset;
	int size   = mh->raw_locns[0].size;

	if ((offset+size) > bufsize)
		return NULL;

	std::string config;
	std::string vol_name;
	int seq_num = -1;
	if (size > 0) {
		config = std::string ((char*) (buffer+4096+offset), size-1);

		seq_num = get_seq_num (config);

		//printf ("seq num = %d\n", seq_num);
		vol_name = get_vol_name (config);
#if 0
		printf ("Config (0x%0x):\n", 4096+offset);
		format_config (config);
		std::cout << "\n" << config << "\n";
#endif
	}

	t = new LvmTable();
	//log_debug ("new LvmTable (%p)\n", t);

	t->bytes_size = ph->device_size_xl;
	t->parent_offset = 0;		//XXX what about if we're in a partition?
	t->bytes_used = 0;
	t->config = config;
	t->seq_num = seq_num;
	t->vol_name = vol_name;
	t->uuid = pv_uuid;

	parent->add_child (t);

	// discover children, either 'pvs' or parse lvm config in header
	fd_pvs (t);

	return t;
}


