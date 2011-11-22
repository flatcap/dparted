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
#include <vector>
#include <string>
#include <parted/parted.h>

#include "container.h"
#include "disk.h"
#include "filesystem.h"
#include "lvm.h"
#include "partition.h"

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
int disk_get_list (std::vector<Container*> &disks)
{
	PedDevice *dev = NULL;
	PedDisk *disk = NULL;
	PedDiskType *type = NULL;
	PedPartition *part = NULL;

	ped_device_probe_all();

	while ((dev = ped_device_get_next (dev))) {
		if (dev->type == PED_DEVICE_DM)
			continue;

		Disk *d = new Disk;
		d->model = dev->model;
		d->path  = dev->path;
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
			while ((part = ped_disk_next_partition (disk, part))) {
				Partition *p = new Partition;
				//std::cout << get_partition_type (part->type) << std::endl;
				p->type = get_partition_type (part->type);
				p->num = part->num;
				p->start = part->geom.start;
				p->length = part->geom.length;
				p->end = part->geom.end;
				if (part->fs_type) {
					Filesystem *f = new Filesystem;
					f->type = part->fs_type->name;
					p->children.push_back (f);
				}
				d->children.push_back (p);
			}
			ped_disk_destroy (disk);
		}

		disks.push_back (d);
	}

	ped_device_free_all();
	return 0;
}

/**
 * main
 */
int main (int argc, char *argv[])
{
	std::vector<Container*> disks;
	unsigned int i;

	disk_get_list (disks);
	for (i = 0; i < disks.size(); i++) {
		disks[i]->Dump();
		delete disks[i];
	}

	return 0;
}
