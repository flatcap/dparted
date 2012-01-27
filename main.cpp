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
#include <stdlib.h>
#include <parted/parted.h>
#include <sys/types.h>
#include <fcntl.h>

#include <map>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

#include "container.h"
#include "disk.h"
#include "extended.h"
#include "file.h"
#include "filesystem.h"
#include "gpt.h"
#include "identify.h"
#include "linear.h"
#include "loop.h"
#include "mirror.h"
#include "msdos.h"
#include "partition.h"
#include "segment.h"
#include "stripe.h"
#include "table.h"
#include "volumegroup.h"
#include "volume.h"

#include "utils.h"
#include "stringnum.h"

struct queue_item {
	std::string name;
	Container *item;
	std::vector<std::string> requires;
};

std::queue<Container*> probe_queue;

/**
 * queue_add_probe
 */
void queue_add_probe (Container *item)
{
	if (!item)
		return;

	probe_queue.push (item);
	//printf ("QUEUE has %lu items\n", probe_queue.size());
}

/**
 * get_partition_type
 */
std::string get_partition_type (int type)
{
	std::string s;

	if (type == PED_PARTITION_NORMAL)    s += "Normal";
	//if (type &  PED_PARTITION_LOGICAL)   s += "Logical ";
	if (type &  PED_PARTITION_EXTENDED)  s += "Extended ";
	//if (type &  PED_PARTITION_FREESPACE) s += "Freespace ";
	if (type &  PED_PARTITION_FREESPACE) s += "empty";
	//if (type &  PED_PARTITION_METADATA)  s += "Metadata ";
	if (type &  PED_PARTITION_METADATA)  s += "reserved";
	if (type &  PED_PARTITION_PROTECTED) s += "Protected ";

	return s;
}

/**
 * disk_get_list
 */
unsigned int disk_get_list (const char *disk_list[], Container &disks)
{
	PedDevice *dev = NULL;
	PedDisk *disk = NULL;
	PedDiskType *type = NULL;
	PedPartition *part = NULL;
	Extended *extended = NULL;
	int reserved = 1;
	int i;

	//ped_device_probe_all();

	//while ((dev = ped_device_get_next (dev))) {
	for (i = 0; disk_list[i]; i++) {
		dev = ped_device_get (disk_list[i]);
		if (!dev)
			continue;

		//printf ("dev->path = %s\n", dev->path);
		if (dev->type == PED_DEVICE_DM)
			continue;

		Disk *d = new Disk;
		//d->parent = &disks;
		d->name = dev->model;
		d->device = dev->path;
		//d->type  = dev->type;
		//RAR if they differ, take the larger one
		d->block_size = dev->sector_size;
		//d->phys_sector_size = dev->phys_sector_size;

		//define size of disk, then add children's size to bytes_used
		d->bytes_size = dev->length * dev->sector_size;
		d->bytes_used = 0;

		d->hw_cylinders = dev->hw_geom.cylinders;
		d->hw_heads     = dev->hw_geom.heads;
		d->hw_sectors   = dev->hw_geom.sectors;

		d->bios_cylinders = dev->bios_geom.cylinders;
		d->bios_heads     = dev->bios_geom.heads;
		d->bios_sectors   = dev->bios_geom.sectors;

		d->host = dev->host;
		d->did  = dev->did;

		//if (dev->open_count)    printf  ("open count    = %d\n",  dev->open_count);
		//if (dev->read_only)     printf  ("read-only     = %d\n",  dev->read_only);
		//if (dev->external_mode) printf  ("external mode = %d\n",  dev->external_mode);
		//if (dev->dirty)         printf  ("dirty         = %d\n",  dev->dirty);
		//if (dev->boot_dirty)    printf  ("boot dirty    = %d\n",  dev->boot_dirty);

		type = ped_disk_probe (dev);
		if (type) {
			Table *table = NULL;

			if (strcmp (type->name, "msdos") == 0) {
				table = new Msdos;
			} else if (strcmp (type->name, "gpt") == 0) {
				table = new Gpt;
			} else {
				//XXX error
				continue;
			}

			//table->parent = d;
			table->bytes_size = d->bytes_size;
			//table->bytes_used = d->bytes_size;
			//table->device = d->device;
			table->device_offset = 0;
			table->name = type->name;

			d->add_child (table);

			//printf ("name = %s\n", type->name);			// msdos
			//printf ("features = %d\n", type->features);
			disk = ped_disk_new (dev);
			extended = NULL;
			bool prev_metadata = false;
			while ((part = ped_disk_next_partition (disk, part))) {
				if (part->type & PED_PARTITION_METADATA) {
					if (prev_metadata) {
						// merge with previous metadata declaration
						Container *parent = NULL;
						if (part->type & PED_PARTITION_LOGICAL) {
							parent = extended;
						} else {
							parent = d;
						}
						Container *meta = parent->children.back();
						meta->bytes_size += part->geom.length * 512;
						meta->bytes_used += part->geom.length * 512;
						parent->bytes_used += part->geom.length * 512;
						prev_metadata = false;
						continue;
					} else {
						//prev_metadata = true;	//RAR coalesce metadata
					}
				} else {
					prev_metadata = false;
				}

				Container *p = NULL;
				if ((part->type & PED_PARTITION_FREESPACE) ||
				    (part->type & PED_PARTITION_METADATA)) {
					p = new Partition;
					//XXX type = metadata/reserved
				} else if (part->type & PED_PARTITION_EXTENDED) {
					p = new Extended;
				} else {
					p = new Partition; // just a normal data partition
				}

				//p->parent = table;
				//std::cout << get_partition_type (part->type) << std::endl;
				if (part->type == PED_PARTITION_EXTENDED) {
					extended = static_cast<Extended*>(p);
					extended->type = "extended";
				}
				p->name = get_partition_type (part->type);
				//RAR nec? p->num = part->num;
				//printf ("Num = %d\n", part->num);

				p->bytes_size = part->geom.length * 512; //RAR need to ask the disk for the multiplicand
				//RAR nec? p->end = part->geom.end;
				//RAR nec? p->start = part->geom.start;
				p->device_offset = part->geom.start * 512; // RAR

				if (part->num > 0) {
					p->device = part->geom.dev->path;
					p->device += ('0' + part->num);
					p->name = p->device.substr (5);
				} else {
					if (p->name == "empty") {
						p->type = "empty";
						if (part->type & PED_PARTITION_LOGICAL) {
							extended->bytes_used -= p->bytes_size;
						} else {
							table->bytes_used -= p->bytes_size;
						}
					} else {
						p->type = "metadata";
						p->name = "reserved_";
						p->name += ('a' + reserved);
						p->bytes_used = p->bytes_size;
						reserved++;
						if (part->type & PED_PARTITION_LOGICAL) {
							p->device = extended->device;
						}
					}
					//p->device = "";
				}
				if (part->fs_type) {
					Filesystem *f = new Filesystem;
					//f->parent = p;
					f->name = part->fs_type->name;
					f->bytes_size = p->bytes_size;
					f->bytes_used = p->bytes_size;
					f->device = p->device;
					f->device_offset = 0;
					p->add_child (f);
				} else {
					if (part->type == PED_PARTITION_LOGICAL) {
						//fprintf (stderr, "no filesystem on %s (%u)\n", p->device.c_str(), part->type);
						//std::string fs = get_fs (p->device, 0);
						//fprintf (stderr, "fs = %s\n", fs.c_str());
					}
				}
				if (part->type & PED_PARTITION_LOGICAL) {
					extended->add_child (p);
					//p->device_offset -= extended->device_offset;
					if (part->num < 0) {
						p->device = d->device;
					}
				} else {
					table->add_child (p);
				}
			}
			ped_disk_destroy (disk);
		}

		disks.add_child (d);
	}

	ped_device_free_all();
	return disks.children.size();
}

/**
 * logicals_get_list
 */
unsigned int logicals_get_list (Container &disks)
{
	std::string command;
	std::string output;
	std::string error;
	VolumeGroup *vg = NULL;
	Volume *vol = NULL;
	std::map<std::string, VolumeGroup*> vg_lookup;
	std::map<std::string, Segment*> vg_seg_lookup;
	std::map<std::string, Segment*> vol_seg_lookup;
	unsigned int i;
	//VolumeSegment vol_seg;
	std::vector<std::string> lines;
	std::map<std::string,StringNum> tags;

	command = "vgs --unquoted --separator='\t' --units=b --nosuffix --nameprefixes --noheadings --options vg_name,pv_count,lv_count,vg_attr,vg_size,vg_free,vg_uuid,vg_extent_size,vg_extent_count,vg_free_count,vg_seqno,pv_name";
	execute_command (command, output, error);
	//printf ("%s\n", command.c_str());
	//printf ("%s\n", output.c_str());

	lines.clear();
	explode ("\n", output, lines);

	//printf ("Volume Groups\n");
	for (i = 0; i < lines.size(); i++) {
		parse_tagged_line ((lines[i]), tags);

		std::string vg_uuid = tags["LVM2_VG_UUID"];
		std::string pv_name = tags["LVM2_PV_NAME"];

		vg = vg_lookup[vg_uuid];
		if (vg == NULL) {
			vg = new VolumeGroup;
			vg->parent = &disks;
			vg->device = "/dev/dm-0"; //RAR what?

			vg->name		= tags["LVM2_VG_NAME"];
			vg->pv_count		= tags["LVM2_PV_COUNT"];
			vg->lv_count		= tags["LVM2_LV_COUNT"];
			vg->vg_attr		= tags["LVM2_VG_ATTR"];
			vg->bytes_size		= tags["LVM2_VG_SIZE"];
			//vg->vg_free		= tags["LVM2_VG_FREE"];
			vg->uuid		= vg_uuid;
			vg->block_size		= tags["LVM2_VG_EXTENT_SIZE"];
			vg->vg_extent_count	= tags["LVM2_VG_EXTENT_COUNT"];
			vg->vg_free_count	= tags["LVM2_VG_FREE_COUNT"];
			vg->vg_seqno		= tags["LVM2_VG_SEQNO"];

			disks.add_child (vg);

			vg_lookup[vg->uuid] = vg;
		}

		std::string seg_id = vg->name + ":" + pv_name;

		//printf ("\t%s\n", seg_id.c_str());

		Container *cont = disks.find_device (pv_name);
		//printf ("cont for %s = %p\n", pv_name.c_str(), cont);

		Segment *vg_seg = new Segment;
		vg_seg->bytes_size = cont->bytes_size;
		vg_seg->type = "vg segment";
		vg_seg->block_size = vg->block_size;
		vg_seg->uuid = vg_uuid;
		vg_seg->name = vg->name;

		cont->add_child (vg_seg);

		//printf ("lookup uuid %s\n", vg_uuid.c_str());
		vg->add_segment (vg_seg);

		//printf ("whole = %p\n", vg_seg->whole);

		//printf ("pv_name = %s\n", pv_name.c_str());
		vg_seg_lookup[pv_name] = vg_seg;
#if 0
		Partition *reserved1 = new Partition;
		//XXX type = metadata/reserved
		// get size from LVM2_PE_START
		reserved1->bytes_size = 1048576;
		reserved1->bytes_used = 1048576;
		reserved1->device_offset = 0;
		reserved1->type = "metadata";
		vg_seg->add_child (reserved1);
#endif
		//RAR need to calculate this from vg_seg size and vg block size
#if 0
		Partition *reserved2 = new Partition;
		//XXX type = metadata/reserved
		// get size from LVM2_PV_PE_ALLOC_COUNT and LVM2_PE_START
		reserved2->bytes_size = 3145728;
		reserved2->bytes_used = 3145728;
		reserved2->device_offset = cont->bytes_size - reserved2->bytes_size;
		reserved2->type = "reserved";
		vg_seg->add_child (reserved2);
#endif
	}
	//printf ("\n");

#if 0
	std::map<std::string,VolumeGroup*>::iterator it;
	printf ("Volume Group map:\n");
	for (it = vg_lookup.begin(); it != vg_lookup.end(); it++) {
		VolumeGroup *vg1 = (*it).second;
		printf ("\t%s => %s\n", (*it).first.c_str(), vg1->name.c_str());
	}
	printf ("\n");
#endif
#if 0
	std::map<std::string,Segment*>::iterator it2;
	printf ("Volume Group Segments map:\n");
	for (it2 = vg_seg_lookup.begin(); it2 != vg_seg_lookup.end(); it2++) {
		Segment *s1 = (*it2).second;
		printf ("\t%s => %s\n", (*it2).first.c_str(), s1->name.c_str());
	}
	printf ("\n");
#endif

	command = "pvs --unquoted --separator='\t' --units=b --nosuffix --nameprefixes --noheadings --options pv_name,lv_name,lv_attr,vg_uuid,vg_name,segtype,pvseg_start,seg_start_pe,pvseg_size,devices";
	execute_command (command, output, error);
	//printf ("%s\n", command.c_str());
	//printf ("%s\n", output.c_str());

	lines.clear();
	explode ("\n", output, lines);

	//printf ("Physical extents\n");
	for (i = 0; i < lines.size(); i++) {
		parse_tagged_line ((lines[i]), tags);

		std::string dev     = tags["LVM2_PV_NAME"];	// /dev/loop0
		std::string vg_uuid = tags["LVM2_VG_UUID"];
		std::string vg_name = tags["LVM2_VG_NAME"];
		std::string lv_name = tags["LVM2_LV_NAME"];
		std::string lv_attr = tags["LVM2_LV_ATTR"];
		std::string lv_type = tags["LVM2_SEGTYPE"];
		std::string pv_name = tags["LVM2_PV_NAME"];
		std::string start   = tags["LVM2_PVSEG_START"];

		if ((lv_attr[0] == 'i') || (lv_attr[0] == 'l')) {	// mirror image/log
			// Strip the []'s
			lv_name = lv_name.substr (1, lv_name.length() - 2);
		}

		std::string seg_id = vg_name + ":" + lv_name + ":" + pv_name + "(" + start + ")";
		//printf ("\t%s\n", seg_id.c_str());

		//printf ("lookup uuid %s\n", vg_uuid.c_str());
		VolumeGroup *vg = vg_lookup[vg_uuid];
		//printf ("vg extent size = %ld\n", vg->block_size);

		Segment *vg_seg = vg_seg_lookup[dev]; //XXX this should exist
		if (!vg_seg)
			continue;

		Segment *vol_seg = new Segment;
		vol_seg->name = tags["LVM2_LV_NAME"];
		vol_seg->type = lv_type;			// striped, etc

		vol_seg->bytes_size = tags["LVM2_PVSEG_SIZE"];
		vol_seg->bytes_size *= vg->block_size;
		vol_seg->device_offset = tags["LVM2_PVSEG_START"];

		vol_seg->whole = NULL; //RAR we don't know this yet

		//printf ("whole = %p\n", vg_seg->whole);

		vg_seg->add_child (vol_seg);

		if (lv_type != "free") {
			vol_seg_lookup[seg_id] = vol_seg;
		}
	}
	//printf ("\n");

#if 0
	std::map<std::string,Segment*>::iterator it3;
	printf ("Volume Segments map:\n");
	for (it3 = vol_seg_lookup.begin(); it3 != vol_seg_lookup.end(); it3++) {
		Segment *s1 = (*it3).second;
		printf ("\t%s => %s\n", (*it3).first.c_str(), s1->name.c_str());
	}
	printf ("\n");
#endif

	command = "lvs --all --unquoted --separator='\t' --units=b --nosuffix --noheadings --nameprefixes --sort lv_kernel_minor --options vg_uuid,vg_name,lv_name,lv_attr,mirror_log,lv_uuid,lv_size,lv_path,lv_kernel_major,lv_kernel_minor,seg_count,segtype,stripes,stripe_size,seg_start,seg_start_pe,seg_size,seg_pe_ranges,devices";
	execute_command (command, output, error);
	//printf ("%s\n", command.c_str());
	//printf ("%s\n", output.c_str());

	lines.clear();
	explode ("\n", output, lines);

	std::map<std::string,Container*> q_easy;
	std::vector<struct queue_item> q_hard;

	//printf ("Volumes:\n");
	for (i = 0; i < lines.size(); i++) {
		parse_tagged_line ((lines[i]), tags);

		std::string vg_uuid = tags["LVM2_VG_UUID"];	//printf ("vg_uuid = %s\n", vg_uuid.c_str());
		std::string vg_name = tags["LVM2_VG_NAME"];	//printf ("vg_name = %s\n", vg_name.c_str());
		std::string lv_name = tags["LVM2_LV_NAME"];	//printf ("lv_name = %s\n", lv_name.c_str());
		std::string lv_attr = tags["LVM2_LV_ATTR"];	//printf ("lv_attr = %s\n", lv_attr.c_str());
		std::string lv_uuid = tags["LVM2_LV_UUID"];	//printf ("lv_uuid = %s\n", lv_uuid.c_str());
		std::string devices = tags["LVM2_DEVICES"];	//printf ("devices = %s\n", devices.c_str());
		std::string lv_type = tags["LVM2_SEGTYPE"];
		std::string mirrlog = tags["LVM2_MIRROR_LOG"];

		std::vector<std::string> device_list;
		explode (",", devices, device_list);

		if ((lv_attr[0] == 'i') || (lv_attr[0] == 'l')) {	// mirror image/log
			// Strip the []'s
			lv_name = lv_name.substr (1, lv_name.length() - 2);
		} else if (lv_attr[0] == 'm') {
			// Add an extra dependency for mirrors
			std::string dep_name = mirrlog + "(0)";
			//printf ("ADD DEP: %s\n", dep_name.c_str());
			device_list.push_back (dep_name);
		}

		bool found_everything = true;
		Container *item = NULL;
		if (lv_type == "linear") {
			item = new Linear;
		} else if (lv_type == "striped") {
			item = new Stripe;
		} else if (lv_type == "mirror") {
			item = new Mirror;
		} else {
			printf ("UNKNOWN type %s\n", lv_type.c_str());
			continue;
		}

		item->name = lv_name;			//RAR and other details...
		item->parent = vg_lookup[vg_uuid];	//XXX until we know better

		for (unsigned int k = 0; k < device_list.size(); k++) {
			std::string seg_id = vg_name + ":" + lv_name + ":" + device_list[k];
			Segment *vol_seg = vol_seg_lookup[seg_id];
#if 1
			//vol_seg_lookup.erase (vol_seg_lookup.find(seg_id));
			//printf ("\t%s ", lv_attr.c_str());
			if (vol_seg) {
				//printf ("\t%s => \e[32m%s\e[0m\n", seg_id.c_str(), vol_seg->name.c_str());
				if (lv_type == "striped") {
					Linear *lin = new Linear;
					lin->name = vol_seg->name; // XXX copy other details
					item->add_child (lin);
					// need to link to segments
				}
			} else {
				found_everything = false;
				//printf ("\t%s => \e[31m%s\e[0m\n", seg_id.c_str(), "MISSING");
			}
#endif
		}

		// search for all segments
		// all found?
		//	yes -> easy
		//	no  -> hard

		std::string sstart = tags["LVM2_SEG_START_PE"];
		//printf ("SEG_START = %s\n", sstart.c_str());
		std::string seg_id = vg_name + ":" + lv_name + "(" + sstart + ")"; //RAR uniq?
		if (found_everything) {
			q_easy[seg_id] = item;
		} else {
			struct queue_item i = { seg_id, item, device_list };
			for (unsigned int m = 0; m < i.requires.size(); m++) {
				// prefix with volume name
				i.requires[m] = vg_name + ":" + i.requires[m];
			}
			q_hard.push_back (i);
		}

		continue;
#if 0
		if ((lv_attr[0] == 'i') || (lv_attr[0] == 'l')) { // mirror (i)mage or (l)og
			// De-mangle the lv_name
			size_t ending = 0;
			if (lv_attr[0] == 'i')
				ending = lv_name.rfind ("_mimage_");
			else
				ending = lv_name.rfind ("_mlog");

			lv_name = lv_name.substr (1, ending - 1);
			//printf ("lv_name = %s\n", lv_name.c_str());
		}
#endif
		vg = vg_lookup[vg_uuid];			//printf ("\tlookup %s  vg = %s\n", vg_uuid.c_str(), vg->name.c_str());

		vol = static_cast<Volume*>(vg->find_name (lv_name));
		if (!vol) {
			//printf ("new volume\n");
			vol = new Volume;
			vol->name    = lv_name;
			vol->uuid    = lv_uuid;	//RAR except for mirrors
			vol->lv_attr = lv_attr;
			vg->add_child (vol);

			vol->device       = tags["LVM2_LV_PATH"];
			vol->bytes_size   = tags["LVM2_LV_SIZE"];
			vol->kernel_major = tags["LVM2_LV_KERNEL_MAJOR"];
			vol->kernel_minor = tags["LVM2_LV_KERNEL_MINOR"];

			if ((lv_attr[0] != 'i') && (lv_attr[0] != 'l')) { // mirror (i)mage or (l)og
				std::string fs_type;
				fs_type = get_fs (vol->device, 0);
				//printf ("fs_type = %s\n", fs_type.c_str());
				if (!fs_type.empty()) {
					Filesystem *fs = new Filesystem;
					//fs->bytes_size = vol->bytes_size;	//RAR for now
					fs->name = fs_type;
					fs->parent = vol; //RAR tmp
					fs->device = vol->device; //RAR tmp
					fs->dump();
					vol->add_child (fs);
				}
			}
		}

		if (lv_attr[0] == 'm') {		// mirror
			vol->device       = tags["LVM2_LV_PATH"];
			std::string fs_type;
			fs_type = get_fs (vol->device, 0);
			//printf ("fs_type = %s\n", fs_type.c_str());
			if (!fs_type.empty()) {
				Filesystem *fs = new Filesystem;
				//fs->bytes_size = vol->bytes_size;	//RAR for now
				fs->name = fs_type;
				fs->parent = vol; //RAR tmp
				fs->device = vol->device; //RAR tmp
				fs->dump();
				vol->add_child (fs);
			}
		}

		//printf ("vol = %p\n", vol);

		std::string mlog = tags["LVM2_MIRROR_LOG"];

		std::string seg_count     = tags["LVM2_SEG_COUNT"];	//printf ("\tseg count = %s\n", seg_count.c_str());
		std::string segtype       = tags["LVM2_SEGTYPE"];	//printf ("\tseg type = %s\n", segtype.c_str());
		long        stripes       = tags["LVM2_STRIPES"];	//printf ("\tstripes = %ld\n", stripes);
		std::string stripe_size   = tags["LVM2_STRIPE_SIZE"];	//printf ("\tstripe size = %s\n", stripe_size.c_str());
		std::string seg_pe_ranges = tags["LVM2_SEG_PE_RANGES"];

		//long long seg_start = tags ["LVM2_SEG_START"]
		//printf ("\tseg start = %s\n", seg_start.c_str());

		//long long seg_size = tags["LVM2_SEG_SIZE"];
		//printf ("\tseg size = %s\n", seg_size.c_str());

		std::string pe_device;
		int pe_start  = -1;
		int pe_finish = -1;

		if (lv_attr[0] == 'm') {		// (m)irror
			stripes = 0;			//XXX don't create another segment
		}

		for (int s = 0; s < stripes; s++) {
			extract_dev_range (seg_pe_ranges, pe_device, pe_start, pe_finish, s);
			//printf ("seg pe ranges = %s, %d, %d\n", pe_device.c_str(), pe_start, pe_finish);

			Segment *vg_seg = vg_seg_lookup[pe_device];
			//printf ("vg_seg = %p\n", vg_seg);

			if ((lv_attr[0] == 'i') || (lv_attr[0] == 'l')) { // mirror (i)mage or (l)og
				// Store a reference for later
				std::string mirr_name = tags["LVM2_LV_NAME"];
				mirr_name = mirr_name.substr (1, mirr_name.size() - 2);
				//printf ("storing mirror name: %s\n", mirr_name.c_str());
				vg_seg_lookup[mirr_name] = vg_seg;
			}

			Segment *vol_seg = new Segment;
			vol_seg->type = "volume";

			vol_seg->block_size = vg->block_size;
			vol_seg->bytes_size = vol_seg->block_size * (pe_finish - pe_start);
			vol_seg->bytes_used = vol_seg->bytes_size;

			//vol_seg->volume_offset = seg_start;
			vol_seg->device        = pe_device;
			//vol_seg->segment_size  = seg_size;
			vol_seg->device_offset = pe_start * vg->block_size;
			//printf ("volume offset = %lld, device = %s, seg size = %lld, device offset = %lld\n", vol_seg.volume_offset, vol_seg.device.c_str(), vol_seg.segment_size, vol_seg.device_offset);

			vol_seg->name = lv_name;
			vol_seg->uuid = vol->uuid;
			vol_seg->whole = vol;

			vg_seg->add_child (vol_seg);
			vol->add_segment (vol_seg);
		}

		//printf ("\n");
		//printf ("vg = %s   lv = %s\n", tmp1.c_str(), vol->name.c_str());
	}

	//printf ("\n");

	for (unsigned int p = 0; p < 1; p++) {
#if 0
		std::map<std::string,Container*>::iterator it4;
		printf ("Easy Queue map:\n");
		for (it4 = q_easy.begin(); it4 != q_easy.end(); it4++) {
			Container *c1 = (*it4).second;
			printf ("\t%s => %s\n", (*it4).first.c_str(), c1->name.c_str());
			for (unsigned int n = 0; n < c1->children.size(); n++) {
				printf ("\t\t%s\n", c1->children[n]->name.c_str());
			}
		}
		printf ("\n");
#endif

#if 0
		std::vector<struct queue_item>::iterator it5;
		printf ("Hard Queue map:\n");
		for (it5 = q_hard.begin(); it5 != q_hard.end(); it5++) {
			printf ("\t%s => %p\n", (*it5).name.c_str(), (*it5).item);
			for (unsigned int l = 0; l < (*it5).requires.size(); l++) {
				printf ("\t\t%s\n", (*it5).requires[l].c_str());
			}
		}
		printf ("\n");
#endif
		if (q_hard.size() == 0)
			break;

		//XXX iterate backwards so we can erase things?
		std::vector<struct queue_item>::iterator it6;
		for (it6 = q_hard.begin(); it6 != q_hard.end(); it6++) {
			bool found_all_hard_stuff = true;
			for (unsigned int q = 0; q < (*it6).requires.size(); q++) {
				std::string req_name = (*it6).requires[q];
				Container *req = q_easy[req_name];
				if (req != NULL) {
					//printf ("found req\n");
					(*it6).item->add_child (req);
					q_easy.erase (q_easy.find(req_name));
				} else {
					printf ("DIDN'T found req: %s\n", req_name.c_str());
					found_all_hard_stuff = false;
				}
			}
			if (found_all_hard_stuff) {
				q_easy["done"] = (*it6).item;
				//q_hard.erase (it6);	//RAR can't do erase in for loop!
			}
		}
	}

#if 0
	std::map<std::string,Container*>::iterator it7;
	printf ("Easy Queue map:\n");
	for (it7 = q_easy.begin(); it7 != q_easy.end(); it7++) {
		Container *c1 = (*it7).second;
		printf ("\t%s => %s\n", (*it7).first.c_str(), c1->name.c_str());
		for (unsigned int n = 0; n < c1->children.size(); n++) {
			printf ("\t\t%s\n", c1->children[n]->name.c_str());
		}
	}
	printf ("\n");
#endif

#if 0
	std::vector<struct queue_item>::iterator it8;
	printf ("Hard Queue map:\n");
	for (it8 = q_hard.begin(); it8 != q_hard.end(); it8++) {
		printf ("\t%s => %p\n", (*it8).name.c_str(), (*it8).item);
		for (unsigned int l = 0; l < (*it8).requires.size(); l++) {
			printf ("\t\t%s\n", (*it8).requires[l].c_str());
		}
	}
	printf ("\n");
#endif

#if 1
	std::map<std::string,Container*>::iterator it9;
	for (it9 = q_easy.begin(); it9 != q_easy.end(); it9++) {
		Container *c1 = (*it9).second;
		c1->parent->add_child (c1);
		//vg->add_child (c1);
	}
#endif
	return disks.children.size();
}

/**
 * mounts_get_list
 */
unsigned int mounts_get_list (Container &mounts)
{
	std::string command;
	std::string output;
	std::string error;
	std::vector<std::string> lines;
	unsigned int i;

	command = "grep '^/dev' /proc/mounts";
	execute_command (command, output, error);
	explode ("\n", output, lines);

	for (i = 0; i < lines.size(); i++) {
		std::string line = lines[i];
		printf ("line%d:\n%s\n\n", i, line.c_str());
	}

	return mounts.children.size();
}

/**
 * main
 */
int main (int argc, char *argv[])
{
	Container disks;
	Container *item = NULL;

	const char *disk_list[] = {
		//"/dev/sda",
		//"/dev/sdb",
		"/dev/sdc",
		//"/dev/sdd",
		//"/dev/loop0", "/dev/loop1", "/dev/loop2",
		//"/dev/loop3",
		//"/dev/loop4",
		//"/dev/loop5", "/dev/loop6", "/dev/loop7",
		//"/dev/loop8", "/dev/loop9", "/dev/loop10", "/dev/loop11", "/dev/loop12", "/dev/loop13", "/dev/loop14", "/dev/loop15",
		//"/var/lib/libvirt/images/f16.img",
		NULL
	};

	for (int i = 0; disk_list[i]; i++) {
		Block::probe (disk_list[i], disks);
	}

	unsigned char *buffer = NULL;
	int bufsize = 4096;
	long long count;
	long long seek;
	int fd;

	buffer = (unsigned char*) malloc (bufsize);
	if (!buffer)
		return 1;

	while ((item = probe_queue.front())) {
#if 0
		printf ("queued item: '%s'\n", item->name.c_str());
#endif
		probe_queue.pop();
		//printf ("QUEUE has %lu items\n", probe_queue.size());

		std::string s1;
		std::string s2;
		s1 = get_size (item->device_offset);
		s2 = get_size (item->bytes_size);
#if 0
		printf ("\tdevice     = %s\n",        item->device.c_str());
		printf ("\toffset     = %lld (%s)\n", item->device_offset, s1.c_str());
		printf ("\ttotal size = %lld (%s)\n", item->bytes_size, s2.c_str());
#endif

		fd = open (item->device.c_str(), O_RDONLY);
		if (!fd) {
			printf ("can't open device %s\n", item->device.c_str());
			continue;
		}

		//printf ("reading from:\n");
		//printf ("\tdevice = %s\n", item->device.c_str());
		//printf ("\tdevice_offset = %lld\n", item->device_offset);

		seek = lseek (fd, item->device_offset, SEEK_SET);
		if (seek != item->device_offset) {
			printf ("seek failed (%lld)\n", seek);
			close (fd);
			continue;
		}
		//printf ("seek succeeded\n"); fflush (stdout);

		count = read (fd, buffer, bufsize);
		if (count != bufsize) {
			printf ("read failed for %s (%d bytes)\n", item->device.c_str(), bufsize);
			close (fd);
			continue;
		}
		//printf ("read succeeded\n"); fflush (stdout);

		//printf ("\t%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", buffer[1070], buffer[1071], buffer[1072], buffer[1073], buffer[1074], buffer[1075], buffer[1076], buffer[1077], buffer[1078], buffer[1079], buffer[1080], buffer[1081], buffer[1082], buffer[1083], buffer[1084], buffer[1085]);

		close (fd);

#if 1
		Table *t = Table::probe (item, buffer, bufsize);
		if (t) {
			printf ("\ttable: %s\n", t->name.c_str());
			printf ("\t\tuuid = %s\n", t->uuid.c_str());
			//delete t;
			continue;
		}
#endif

#if 0
		Filesystem *f = Filesystem::probe (item, buffer, bufsize);
		if (f) {
			printf ("\tfilesystem: %s\n", f->name.c_str());
			//delete f;
			continue;
		}
#endif

		//logicals...
	}

#if 0
	disk_get_list (disk_list, disks);
	logicals_get_list (disks);
#endif

#if 0
	std::string dot;
	dot += "digraph disks {\n";
	dot += "graph [ rankdir = \"TB\", bgcolor = white ];\n";
	dot += "node [ shape = record, color = black, fillcolor = lightcyan, style = filled ];\n";

	unsigned int i;
	for (i = 0; i < disks.children.size(); i++) {
		dot += disks.children[i]->dump_dot();
	}
	dot += "\n};";

	printf ("%s\n", dot.c_str());
#endif

	return 0;
}

