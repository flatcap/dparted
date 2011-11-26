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
	if (type &  PED_PARTITION_LOGICAL)   s += "Logical ";
	if (type &  PED_PARTITION_EXTENDED)  s += "Extended ";
	if (type &  PED_PARTITION_FREESPACE) s += "Freespace ";
	if (type &  PED_PARTITION_METADATA)  s += "Metadata ";
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
		d->model = dev->model;
		d->device  = dev->path;
		d->type  = dev->type;
		d->sector_size = dev->sector_size;
		d->phys_sector_size = dev->phys_sector_size;
		d->length = dev->length;

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
			//printf ("name = %s\n", type->name);			// msdos
			//printf ("features = %d\n", type->features);
			disk = ped_disk_new (dev);
			extended = NULL;
			while ((part = ped_disk_next_partition (disk, part))) {
				Partition *p = new Partition;
				p->parent = d;
				//std::cout << get_partition_type (part->type) << std::endl;
				if (part->type == PED_PARTITION_EXTENDED) {
					extended = p;
				}
				p->type = get_partition_type (part->type);
				p->num = part->num;
				p->device = part->geom.dev->path;
				p->start = part->geom.start;
				p->length = part->geom.length;
				p->end = part->geom.end;
				if (part->fs_type) {
					Filesystem *f = new Filesystem;
					f->parent = p;
					f->type = part->fs_type->name;
					f->part = p;
					p->children.push_back (f);
				}
				if (part->type == PED_PARTITION_LOGICAL) {
					extended->children.push_back (p);
				} else {
					d->children.push_back (p);
				}
			}
			ped_disk_destroy (disk);
		}

		disks.children.push_back (d);
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
	unsigned int index = 0;
	std::map<std::string, VolumeGroup*> vg_lookup;
	int i;

	command = "vgs --units=b --nosuffix  --nameprefixes --noheadings --options vg_name,pv_count,lv_count,vg_attr,vg_size,vg_free,vg_uuid,vg_extent_size,vg_extent_count,vg_free_count,vg_seqno";
	execute_command (command, output, error);

	//printf ("%s\n", output.c_str());

	for (i = 0; i < 4; i++) {
		vg = new VolumeGroup;
		vg->parent = &logicals;

		index = output.find ("LVM2_VG_NAME", index);
		vg->vg_name = extract_quoted_string (output, index);

		index = output.find ("LVM2_PV_COUNT", index);
		vg->pv_count = extract_quoted_long (output, index);

		index = output.find ("LVM2_LV_COUNT", index);
		vg->lv_count = extract_quoted_long (output, index);

		index = output.find ("LVM2_VG_ATTR", index);
		vg->vg_attr = extract_quoted_string (output, index);

		index = output.find ("LVM2_VG_SIZE", index);
		vg->vg_size = extract_quoted_long_long (output, index);

		index = output.find ("LVM2_VG_FREE", index);
		vg->vg_free = extract_quoted_long_long (output, index);

		index = output.find ("LVM2_VG_UUID", index);
		vg->vg_uuid = extract_quoted_string (output, index);

		index = output.find ("LVM2_VG_EXTENT_SIZE", index);
		vg->vg_extent_size = extract_quoted_long_long (output, index);

		index = output.find ("LVM2_VG_EXTENT_COUNT", index);
		vg->vg_extent_count = extract_quoted_long_long (output, index);

		index = output.find ("LVM2_VG_FREE_COUNT", index);
		vg->vg_free_count = extract_quoted_long_long (output, index);

		index = output.find ("LVM2_VG_SEQNO", index);
		vg->vg_seqno = extract_quoted_long (output, index);

		logicals.children.push_back (vg);

		vg_lookup[vg->vg_uuid] = vg;
	}

	std::map<std::string,VolumeGroup*>::iterator it;

#if 0
	printf ("map:\n");
	for (it = vg_lookup.begin(); it != vg_lookup.end(); it++) {
		printf ("\t%s => %p\n", (*it).first.c_str(), (*it).second);
	}
	printf ("\n");
#endif

	command = "lvs --all --units=b --nosuffix --noheadings --nameprefixes --options vg_uuid,lv_name,lv_attr,lv_size,lv_path,lv_kernel_major,lv_kernel_minor,seg_count,segtype,stripes,stripe_size,seg_start,seg_size,seg_pe_ranges,devices";
	execute_command (command, output, error);

	//printf ("%s\n", output.c_str());

	index = 0;
	for (i = 0; i < 27; i++) {
		vol = new Volume;

		index = output.find ("LVM2_VG_UUID", index);
		std::string vg_uuid = extract_quoted_string (output, index);

		index = output.find ("LVM2_LV_NAME", index);
		vol->lv_name = extract_quoted_string (output, index);

		index = output.find ("LVM2_LV_ATTR", index);
		vol->lv_attr = extract_quoted_string (output, index);

		index = output.find ("LVM2_LV_SIZE", index);
		vol->lv_size = extract_quoted_long_long (output, index);

		index = output.find ("LVM2_LV_PATH", index);
		vol->lv_path = extract_quoted_string (output, index);

		index = output.find ("LVM2_LV_KERNEL_MAJOR", index);
		vol->kernel_major = extract_quoted_long (output, index);

		index = output.find ("LVM2_LV_KERNEL_MINOR", index);
		vol->kernel_minor = extract_quoted_long (output, index);

		//printf ("vg = %s   lv = %s\n", tmp1.c_str(), vol->name.c_str());

		vol->parent = vg_lookup[vg_uuid];
		vol->parent->children.push_back (vol);
	}

	return logicals.children.size();
}

/**
 * mounts_get_list
 */
unsigned int mounts_get_list (Container &mounts)
{
	return mounts.children.size();
}

/**
 * main
 */
int main (int argc, char *argv[])
{
	//Container disks;
	//disk_get_list (disks);
	//disks.Dump(-8);

	Container logicals;
	logicals_get_list (logicals);
	logicals.Dump(-8);

	//Container mounts;
	//mounts_get_list (mounts);
	//mounts.Dump(-8);

	return 0;
}
