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
		//printf ("dev = %p\n", dev);
		printf ("model = %s\n", dev->model);
		printf ("path = %s\n", dev->path);
		printf ("type = %d\n", dev->type);
		printf ("sector size = %lld\n", dev->sector_size);
		printf ("physical sector size = %lld\n", dev->phys_sector_size);
		printf ("length (sectors) = %lld\n", dev->length);
		if (dev->open_count)    printf  ("open count           = %d\n",  dev->open_count);
		if (dev->read_only)     printf  ("read-only            = %d\n",  dev->read_only);
		if (dev->external_mode) printf  ("external mode        = %d\n",  dev->external_mode);
		if (dev->dirty)         printf  ("dirty                = %d\n",  dev->dirty);
		if (dev->boot_dirty)    printf  ("boot dirty           = %d\n",  dev->boot_dirty);
		printf ("hardware: cyl = %d, heads = %d, sectors = %d\n", dev->hw_geom.cylinders, dev->hw_geom.heads, dev->hw_geom.sectors);
		printf ("bios: cyl = %d, heads = %d, sectors = %d\n", dev->bios_geom.cylinders, dev->bios_geom.heads, dev->bios_geom.sectors);
		printf ("host = %d\n", dev->host);
		printf ("did = %d\n", dev->did);
		type = ped_disk_probe (dev);
		if (type) {
			printf ("name = %s\n", type->name);
			printf ("features = %d\n", type->features);
			printf ("partitions:\n");
			disk = ped_disk_new (dev);
			while ((part = ped_disk_next_partition (disk, part))) {
				printf ("\t----------------------\n");
				printf ("\tnum = %d\n", part->num);
				printf ("\ttype = "); print_partition_type (part->type); printf ("\n");
				printf ("\tstart = %lld\n", part->geom.start);
				printf ("\tlength = %lld\n", part->geom.length);
				printf ("\tend = %lld\n", part->geom.end);
				if (part->fs_type)
					printf ("\tfs type = %s\n", part->fs_type->name);
			}
		}
		printf ("\n");
	}

	return 0;
}

