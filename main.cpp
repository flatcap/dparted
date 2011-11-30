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
#include <map>
#include <parted/parted.h>

#include "container.h"
#include "disk.h"
#include "filesystem.h"
#include "mount.h"
#include "msdos.h"
#include "partition.h"
#include "volumegroup.h"
#include "volume.h"

#include "utils.h"

/**
 * run_command
 */
void run_command (void)
{
	std::string output;
	std::string error;
	std::string command = "vgs --units=b --nosuffix --noheading -a";

	if (execute_command (command, output, error))
		return;

	if (output.size() == 0)
		return;

	printf ("%s\n", output.c_str());
	/*
	std::vector<std::string> temp_arr;
	Utils::tokenize(output, temp_arr, "\n");
	for (unsigned int k = 0; k < temp_arr.size(); k++)
	{
		std::string temp = Utils::regexp_label(output, "^[^/]*(/dev/[^\t ]*)");
		if (temp.size() > 0)
			swraid_devices.push_back(temp);
	}
	*/
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
	if (type &  PED_PARTITION_FREESPACE) s += "<empty>";
	//if (type &  PED_PARTITION_METADATA)  s += "Metadata ";
	if (type &  PED_PARTITION_METADATA)  s += "<reserved>";
	if (type &  PED_PARTITION_PROTECTED) s += "Protected ";

	return s;
}

/**
 * disk_get_list
 */
unsigned int disk_get_list (Container &disks)
{
	PedDevice *dev = NULL;
	PedDisk *disk = NULL;
	PedDiskType *type = NULL;
	PedPartition *part = NULL;
	Partition *extended = NULL;

	ped_device_probe_all();

	while ((dev = ped_device_get_next (dev))) {
		if (dev->type == PED_DEVICE_DM)
			continue;

		Disk *d = new Disk;
		d->parent = &disks;
		d->name = dev->model;
		d->device  = dev->path;
		//d->type  = dev->type;
		//RAR if they differ, take the larger one
		d->block_size = dev->sector_size;
		//d->phys_sector_size = dev->phys_sector_size;

		d->bytes_size = dev->length * dev->sector_size;
		d->bytes_used = 0;
		//RAR define size of disk
		//RAR then add children's size to bytes_used

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

			m->parent = d;
			m->bytes_size = d->bytes_size;
			//m->bytes_used = d->bytes_size;
			m->name = "msdos";

			d->add_child (m);

			//printf ("name = %s\n", type->name);			// msdos
			//printf ("features = %d\n", type->features);
			disk = ped_disk_new (dev);
			extended = NULL;
			while ((part = ped_disk_next_partition (disk, part))) {
				Partition *p = new Partition;
				p->parent = m;
				//std::cout << get_partition_type (part->type) << std::endl;
				if (part->type == PED_PARTITION_EXTENDED) {
					extended = p;
				}
				p->name = get_partition_type (part->type);
				p->num = part->num;
				//printf ("Num = %d\n", part->num);
				p->device = part->geom.dev->path;

				p->bytes_size = part->geom.length * 512; //RAR need to ask the disk for the multiplicand
				p->end = part->geom.end;
				p->start = part->geom.start;
				p->device_offset = p->start * 512; // RAR

				if (part->num > 0) {
					p->device += ('0' + part->num);
					p->name = p->device.substr (5);
				} else {
					if (p->name == "<empty>") {
						p->type = "\e[37m<empty>\e[0m";
						p->name = "<empty>";
						m->bytes_used -= p->bytes_size;
					} else {
						p->type = "\e[37m<metadata>\e[0m";
						p->name = "<reserved>";
					}
					p->device = "";
					p->bytes_used = p->bytes_size;
				}
				if (part->fs_type) {
					Filesystem *f = new Filesystem;
					f->parent = p;
					f->name = part->fs_type->name;
					f->part = p;
					f->bytes_size = p->bytes_size;
					f->bytes_used = p->bytes_size;
					p->add_child (f);
				}
				if (part->type == PED_PARTITION_LOGICAL) {
					extended->add_child (p);
				} else {
					m->add_child (p);
				}
			}
			ped_disk_destroy (disk);
		}

		disks.add_child (d);

		break;//RAR
	}

	ped_device_free_all();
	return disks.children.size();
}

/**
 * logicals_get_list
 */
unsigned int logicals_get_list (Container &logicals)
{
	std::string command;
	std::string output;
	std::string error;
	VolumeGroup *vg = NULL;
	Volume *vol = NULL;
	unsigned int index;
	std::map<std::string, VolumeGroup*> vg_lookup;
	unsigned int i;
	unsigned int j;
	VolumeSegment vol_seg;
	std::vector<std::string> lines;

	command = "vgs --units=b --nosuffix  --nameprefixes --noheadings --options vg_name,pv_count,lv_count,vg_attr,vg_size,vg_free,vg_uuid,vg_extent_size,vg_extent_count,vg_free_count,vg_seqno";
	execute_command (command, output, error);
	get_lines (output, lines);

	for (i = 0; i < lines.size(); i++) {
		index = 0;
		std::string line = lines[i];
		//printf ("line%d:\n%s\n\n", i, line.c_str()); fflush (stdout);

		vg = new VolumeGroup;
		vg->parent = &logicals;

		index = line.find ("LVM2_VG_NAME", index);
		vg->vg_name = extract_quoted_string (line, index);

		index = line.find ("LVM2_PV_COUNT", index);
		vg->pv_count = extract_quoted_long (line, index);

		index = line.find ("LVM2_LV_COUNT", index);
		vg->lv_count = extract_quoted_long (line, index);

		index = line.find ("LVM2_VG_ATTR", index);
		vg->vg_attr = extract_quoted_string (line, index);

		index = line.find ("LVM2_VG_SIZE", index);
		vg->vg_size = extract_quoted_long_long (line, index);

		index = line.find ("LVM2_VG_FREE", index);
		vg->vg_free = extract_quoted_long_long (line, index);

		index = line.find ("LVM2_VG_UUID", index);
		vg->vg_uuid = extract_quoted_string (line, index);

		index = line.find ("LVM2_VG_EXTENT_SIZE", index);
		vg->vg_extent_size = extract_quoted_long_long (line, index);

		index = line.find ("LVM2_VG_EXTENT_COUNT", index);
		vg->vg_extent_count = extract_quoted_long_long (line, index);

		index = line.find ("LVM2_VG_FREE_COUNT", index);
		vg->vg_free_count = extract_quoted_long_long (line, index);

		index = line.find ("LVM2_VG_SEQNO", index);
		vg->vg_seqno = extract_quoted_long (line, index);

		logicals.add_child (vg);

		vg_lookup[vg->vg_uuid] = vg;
	}

	//logicals.dump (-8);

#if 0
	std::map<std::string,VolumeGroup*>::iterator it;
	printf ("map:\n");
	for (it = vg_lookup.begin(); it != vg_lookup.end(); it++) {
		VolumeGroup *vg1 = (*it).second;
		printf ("\t%s => %s\n", (*it).first.c_str(), vg1->vg_name.c_str());
	}
	printf ("\n");
#endif

	command = "lvs --all --units=b --nosuffix --noheadings --nameprefixes --options vg_uuid,lv_name,lv_attr,lv_size,lv_path,lv_kernel_major,lv_kernel_minor,seg_count,segtype,stripes,stripe_size,seg_start,seg_size,seg_pe_ranges";
	execute_command (command, output, error);

	lines.clear();
	get_lines (output, lines);

	//printf ("%s\n", output.c_str());

	for (i = 0; i < lines.size(); i++) {
		index = 0;
		std::string line = lines[i];
		//printf ("line%d:\n%s\n\n", i, line.c_str()); fflush (stdout);

		//printf ("Volume Part\n");

		index = line.find ("LVM2_VG_UUID", index);
		std::string vg_uuid = extract_quoted_string (line, index);
		//printf ("\tuuid = %s\n", vg_uuid.c_str());

		vg = vg_lookup[vg_uuid];
		//printf ("\tlookup %s  vg = %s\n", vg_uuid.c_str(), vg->vg_name.c_str());

		index = line.find ("LVM2_LV_NAME", index);
		std::string lv_name = extract_quoted_string (line, index);
		//printf ("\tlv name = %s\n", lv_name.c_str());

		vol = NULL;
		//printf ("\t%s has %lu children to check\n", vg->vg_name.c_str(), vg->children.size());
		for (j = 0; j < vg->children.size(); j++) {
			Volume *tmp = (Volume *) vg->children[j];
			//printf ("\t\tCompare %s to %s\n", tmp->lv_name.c_str(), lv_name.c_str());
			if (tmp->lv_name == lv_name) {
				vol = tmp;
				break;
			}
		}

		if (!vol) {
			//printf ("\tNEW VOLUME %s\n", lv_name.c_str());
			vol = new Volume;
			vg->add_child (vol);
		}

		vol->lv_name = lv_name;

		//printf ("\tvol = %p\n", vol);
		//printf ("lv_name = %s\n", vol->lv_name.c_str());
		//printf ("\n");
		//continue;

		index = line.find ("LVM2_LV_ATTR", index);
		vol->lv_attr = extract_quoted_string (line, index);

		index = line.find ("LVM2_LV_SIZE", index);
		vol->lv_size = extract_quoted_long_long (line, index);

		index = line.find ("LVM2_LV_PATH", index);
		vol->lv_path = extract_quoted_string (line, index);

		index = line.find ("LVM2_LV_KERNEL_MAJOR", index);
		vol->kernel_major = extract_quoted_long (line, index);

		index = line.find ("LVM2_LV_KERNEL_MINOR", index);
		vol->kernel_minor = extract_quoted_long (line, index);

		index = line.find ("LVM2_SEG_COUNT", index);
		std::string seg_count = extract_quoted_string (line, index);
		//printf ("\tseg count = %s\n", seg_count.c_str());

		index = line.find ("LVM2_SEGTYPE", index);
		std::string segtype = extract_quoted_string (line, index);
		//printf ("\tseg type = %s\n", segtype.c_str());

		index = line.find ("LVM2_STRIPES", index);
		long stripes = extract_quoted_long (line, index);
		//printf ("\tstripes = %s\n", stripes.c_str());

		index = line.find ("LVM2_STRIPE_SIZE", index);
		std::string stripe_size = extract_quoted_string (line, index);
		//printf ("\tstripe size = %s\n", stripe_size.c_str());

		index = line.find ("LVM2_SEG_START", index);
		long long seg_start = extract_quoted_long_long (line, index);
		//printf ("\tseg start = %s\n", seg_start.c_str());

		index = line.find ("LVM2_SEG_SIZE", index);
		long long seg_size = extract_quoted_long_long (line, index);
		//printf ("\tseg size = %s\n", seg_size.c_str());

		index = line.find ("LVM2_SEG_PE_RANGES", index);
		std::string seg_pe_ranges = extract_quoted_string (line, index);
		//printf ("\t                0         1         2         3         4         \n");
		//printf ("\t                01234567890123456789012345678901234567890123456789\n");
		//printf ("\tseg pe ranges = %s\n", seg_pe_ranges.c_str());
		std::string pe_device;
		int pe_start  = -1;
		int pe_finish = -1;

		for (int s = 0; s < stripes; s++) {
				extract_dev_range (seg_pe_ranges, pe_device, pe_start, pe_finish, s);
				//printf ("\tseg pe ranges = %s, %d, %d\n", pe_device.c_str(), pe_start, pe_finish);

				vol_seg.volume_offset = seg_start;
				vol_seg.device        = pe_device;
				vol_seg.segment_size  = seg_size;
				vol_seg.device_offset = pe_start * seg_size;

				bool inserted = false;
				for (std::vector<VolumeSegment>::iterator it = vol->segments.begin(); it != vol->segments.end(); it++) {
					if (it->volume_offset > vol_seg.volume_offset) {
						vol->segments.insert (it, vol_seg);
						inserted = true;
						//printf ("inserted\n"); fflush (stdout);
						break;
					}
				}

				if (!inserted) {
					vol->segments.push_back (vol_seg);
					//RAR link these siblings?
				}
		}

		//printf ("\n");
		//printf ("vg = %s   lv = %s\n", tmp1.c_str(), vol->name.c_str());
	}

	return logicals.children.size();
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
	get_lines (output, lines);

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
	printf ("\e[36mDevice       Type          Offset  Name                       Size     Used     Free\e[0m\n");
	disks.dump2();
	//printf ("ContainerType,Device,Name,Blocksize,Label,UUID,Total,Used,Free\n");
	//disks.dump_csv();

	//Container logicals;
	//logicals_get_list (logicals);
	//logicals.dump(-8);

	//Container mounts;
	//mounts_get_list (mounts);
	//mounts.dump(-8);

	return 0;
}
