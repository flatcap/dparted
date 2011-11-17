// gcc -g -Wall -o p p.c -L/lib64 -lparted

#include <stdio.h>
#include <parted/parted.h>

/**
 * print_partition_type
 */
void print_partition_type (int type)
{
	if (type == PED_PARTITION_NORMAL)    printf ("Normal");
	if (type &  PED_PARTITION_LOGICAL)   printf ("Logical ");
	if (type &  PED_PARTITION_EXTENDED)  printf ("Extended ");
	if (type &  PED_PARTITION_FREESPACE) printf ("Freespace ");
	if (type &  PED_PARTITION_METADATA)  printf ("Metadata ");
	if (type &  PED_PARTITION_PROTECTED) printf ("Protected ");
}

int main (int argc, char *argv[])
{
	PedDevice *dev = NULL;
	PedDisk *disk = NULL;
	PedDiskType *type = NULL;
	PedPartition *part = NULL;

	ped_device_probe_all();

	while ((dev = ped_device_get_next (dev))) {
		printf ("dev = %p\n", dev);
		/** A block device - for example, /dev/hda, not /dev/hda3 */
		printf ("\tmodel = %s\n", dev->model);
		printf ("\tpath = %s\n", dev->path);
		printf ("\ttype = %d\n", dev->type);
		type = ped_disk_probe (dev);
		if (type) {
			printf ("\t\tname = %s\n", type->name);
			printf ("\t\tfeatures = %d\n", type->features);
			printf ("\t\tpartitions:\n");
			disk = ped_disk_new (dev);
			while ((part = ped_disk_next_partition (disk, part))) {
				printf ("\t\t\t----------------------\n");
				printf ("\t\t\tnum = %d\n", part->num);
				printf ("\t\t\ttype = "); print_partition_type (part->type); printf ("\n");
				printf ("\t\t\tstart = %lld\n", part->geom.start);
				printf ("\t\t\tlength = %lld\n", part->geom.length);
				printf ("\t\t\tend = %lld\n", part->geom.end);
				if (part->fs_type)
					printf ("\t\t\tfs type = %s\n", part->fs_type->name);
			}
		}
		printf ("\tsector size = %lld\n", dev->sector_size);
		printf ("\tphysical sector size = %lld\n", dev->phys_sector_size);
		printf ("\tlength = %lld\n", dev->length);
		printf ("\thardware: cyl = %d, heads = %d, sectors = %d\n", dev->hw_geom.cylinders, dev->hw_geom.heads, dev->hw_geom.sectors);
		printf ("\tbios: cyl = %d, heads = %d, sectors = %d\n", dev->bios_geom.cylinders, dev->bios_geom.heads, dev->bios_geom.sectors);
		printf ("\thost = %d\n", dev->host);
		printf ("\tdid = %d\n", dev->did);
		printf ("\n");
	}

	return 0;
}

