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
#include <stdlib.h>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <parted/parted.h>

#include "container.h"
#include "disk.h"
#include "extended.h"
#include "filesystem.h"
#include "metadata.h"
#include "mount.h"
#include "msdos.h"
#include "partition.h"
#include "segment.h"
#include "volumegroup.h"
#include "volume.h"

#include "utils.h"
#include "stringnum.h"

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
unsigned int disk_get_list (Container &disks)
{
	const char *disk_list[] = {
		"/dev/sda", "/dev/sdb", "/dev/sdc", "/dev/sdd",
		"/dev/loop0", "/dev/loop1", "/dev/loop2", "/dev/loop3",
		"/dev/loop4",
		//"/dev/loop5", "/dev/loop6", "/dev/loop7",
		NULL
	};
	PedDevice *dev = NULL;
	PedDisk *disk = NULL;
	PedDiskType *type = NULL;
	PedPartition *part = NULL;
	Partition *extended = NULL;
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
			Msdos *m = new Msdos;

			//m->parent = d;
			m->bytes_size = d->bytes_size;
			//m->bytes_used = d->bytes_size;
			//m->device = d->device;
			m->device_offset = 0;
			m->name = "msdos";

			d->add_child (m);

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
					p = new Metadata;
				} else if (part->type & PED_PARTITION_EXTENDED) {
					p = new Extended;
				} else {
					p = new Partition;
				}

				//p->parent = m;
				//std::cout << get_partition_type (part->type) << std::endl;
				if (part->type == PED_PARTITION_EXTENDED) {
					extended = static_cast<Partition*>(p);
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
							m->bytes_used -= p->bytes_size;
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
					m->add_child (p);
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
	std::map<std::string, Segment*> seg_lookup;
	unsigned int i;
	//VolumeSegment vol_seg;
	std::vector<std::string> lines;
	std::map<std::string,StringNum> tags;

	command = "vgs --unquoted --separator='\t' --units=b --nosuffix --nameprefixes --noheadings --options vg_name,pv_count,lv_count,vg_attr,vg_size,vg_free,vg_uuid,vg_extent_size,vg_extent_count,vg_free_count,vg_seqno";
	execute_command (command, output, error);
	//printf ("%s\n", command.c_str());
	//printf ("%s\n", output.c_str());

	lines.clear();
	explode ("\n", output, lines);

	for (i = 0; i < lines.size(); i++) {
		tags.clear();
		parse_tagged_line ((lines[i]), tags);

		vg = new VolumeGroup;
		vg->parent = &disks;
		vg->device = "/dev/dm-0";

		vg->name		= tags["LVM2_VG_NAME"];		//printf ("name = %s\n", vg->name.c_str(;
		vg->pv_count		= tags["LVM2_PV_COUNT"];
		vg->lv_count		= tags["LVM2_LV_COUNT"];
		vg->vg_attr		= tags["LVM2_VG_ATTR"];
		vg->bytes_size		= tags["LVM2_VG_SIZE"];
		//vg->vg_free		= tags["LVM2_VG_FREE"];
		vg->uuid		= tags["LVM2_VG_UUID"];
		vg->block_size		= tags["LVM2_VG_EXTENT_SIZE"];
		vg->vg_extent_count	= tags["LVM2_VG_EXTENT_COUNT"];
		vg->vg_free_count	= tags["LVM2_VG_FREE_COUNT"];
		vg->vg_seqno		= tags["LVM2_VG_SEQNO"];

		disks.add_child (vg);

		vg_lookup[vg->uuid] = vg;
	}

#if 0
	std::map<std::string,VolumeGroup*>::iterator it;
	printf ("map:\n");
	for (it = vg_lookup.begin(); it != vg_lookup.end(); it++) {
		VolumeGroup *vg1 = (*it).second;
		printf ("\t%s => %s\n", (*it).first.c_str(), vg1->name.c_str());
	}
	printf ("\n");
#endif

	command = "pvs --unquoted --separator='\t' --units=b --nosuffix --nameprefixes --noheadings --options pv_name,vg_uuid,vg_name,vg_extent_size,pv_attr,pv_size,pv_free,pv_uuid,dev_size,pe_start,pv_used,pv_pe_count,pv_pe_alloc_count";
	execute_command (command, output, error);
	//printf ("%s\n", command.c_str());
	//printf ("%s\n", output.c_str());

	lines.clear();
	explode ("\n", output, lines);

	for (i = 0; i < lines.size(); i++) {
		tags.clear();
		parse_tagged_line ((lines[i]), tags);

		/* is /dev/sda8 in seg_lookup
		 * no
		 *	find /dev/sda8
		 *	create a vg segment child linked to test
		 *	add segment to seg_lookup: /dev/sda8 -> test
		 * yes
		 *	add new volume segment linked to alpha,beta,etc
		 */

		std::string dev     = tags["LVM2_PV_NAME"];
		std::string vg_uuid = tags["LVM2_VG_UUID"];
		std::string vg_name = tags["LVM2_VG_NAME"];

		Segment *vg_seg = seg_lookup[dev];
		if (vg_seg == NULL) {
			Container *cont = disks.find_device (dev);
			//printf ("cont for %s = %p\n", dev.c_str(), cont);
			vg_seg = new Segment;
			vg_seg->bytes_size = cont->bytes_size;
			vg_seg->name = vg_name;
			vg_seg->type = "volumegroup";
			vg_seg->block_size = tags["LVM2_VG_EXTENT_SIZE"];

			cont->add_child (vg_seg);

			//printf ("lookup uuid %s\n", vg_uuid.c_str());
			VolumeGroup *vg = vg_lookup[vg_uuid];
			vg->add_segment (vg_seg);

			//printf ("whole = %p\n", vg_seg->whole);

			//printf ("dev = %s\n", dev.c_str());
			seg_lookup[dev] = vg_seg;

			Metadata *reserved1 = new Metadata;	// RAR these need to be Segments, too
			// get size from LVM2_PE_START
			reserved1->bytes_size = 1048576;
			reserved1->bytes_used = 1048576;
			reserved1->device_offset = 0;
			reserved1->type = "metadata";
			vg_seg->add_child (reserved1);

			Metadata *reserved2 = new Metadata;
			// get size from LVM2_PV_PE_ALLOC_COUNT and LVM2_PE_START
			reserved2->bytes_size = 3145728;
			reserved2->bytes_used = 3145728;
			reserved2->device_offset = 108037931008;
			reserved2->type = "reserved";
			vg_seg->add_child (reserved2);
		} else {
			//printf ("already got one\n");
		}

#if 0
		Segment *vol_seg = new Segment;
		vol_seg->name = "lvm volume";
#endif
		//printf ("extent size = %g MiB\n", (double) vg_seg->whole->block_size / 1024.0 / 1024.0);
#if 0
		// ignored for now
		std::string x;
		x = tags["LVM2_PV_ATTR"];		//printf ("x = %s\n", x.c_str());
		x = tags["LVM2_PV_SIZE"];		//printf ("x = %s\n", x.c_str());
		x = tags["LVM2_PV_FREE"];		//printf ("x = %s\n", x.c_str());
		x = tags["LVM2_PV_UUID"];		//printf ("x = %s\n", x.c_str());
		x = tags["LVM2_DEV_SIZE"];		//printf ("x = %s\n", x.c_str());
		x = tags["LVM2_PE_START"];		//printf ("x = %s\n", x.c_str());
		x = tags["LVM2_PV_USED"];		//printf ("x = %s\n", x.c_str());
		x = tags["LVM2_PV_PE_COUNT"];		//printf ("x = %s\n", x.c_str());
		x = tags["LVM2_PV_PE_ALLOC_COUNT"];	//printf ("x = %s\n", x.c_str());
#endif

#if 0
		long long pvseg_start = tags["LVM2_PVSEG_START"];	//printf ("x = %s\n", x.c_str());
		long long pvseg_size  = tags["LVM2_PVSEG_SIZE"];	//printf ("x = %s\n", x.c_str());

		vol_seg->bytes_size    = pvseg_size  * vg_seg->whole->block_size;
		vol_seg->bytes_used    = pvseg_size  * vg_seg->whole->block_size;
		vol_seg->device_offset = pvseg_start * vg_seg->whole->block_size;

		vg_seg->add_child (vol_seg);
#endif
	}

	command = "lvs --all --unquoted --separator='\t' --units=b --nosuffix --noheadings --nameprefixes --sort lv_kernel_minor --options vg_uuid,lv_name,lv_attr,mirror_log,lv_uuid,lv_size,lv_path,lv_kernel_major,lv_kernel_minor,seg_count,segtype,stripes,stripe_size,seg_start,seg_size,seg_pe_ranges";
	execute_command (command, output, error);
	//printf ("%s\n", command.c_str());
	//printf ("%s\n", output.c_str());

	lines.clear();
	explode ("\n", output, lines);

	for (i = 0; i < lines.size(); i++) {
		tags.clear();
		parse_tagged_line ((lines[i]), tags);

		std::string vg_uuid = tags["LVM2_VG_UUID"];	//printf ("vg_uuid = %s\n", vg_uuid.c_str());
		std::string lv_name = tags["LVM2_LV_NAME"];	//printf ("lv_name = %s\n", lv_name.c_str());
		std::string lv_attr = tags["LVM2_LV_ATTR"];	//printf ("lv_attr = %s\n", lv_attr.c_str());
		std::string lv_uuid = tags["LVM2_LV_UUID"];	//printf ("lv_uuid = %s\n", lv_uuid.c_str());

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

			Segment *vg_seg = seg_lookup[pe_device];
			//printf ("vg_seg = %p\n", vg_seg);

			if ((lv_attr[0] == 'i') || (lv_attr[0] == 'l')) { // mirror (i)mage or (l)og
				// Store a reference for later
				std::string mirr_name = tags["LVM2_LV_NAME"];
				mirr_name = mirr_name.substr (1, mirr_name.size() - 2);
				//printf ("storing mirror name: %s\n", mirr_name.c_str());
				seg_lookup[mirr_name] = vg_seg;
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

	disk_get_list (disks);
	logicals_get_list (disks);

#if 1
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

