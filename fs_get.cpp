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

#include <string>
#include <cstring>

#include <endian.h>

#include "fs_identify.h"
#include "fs_usage.h"
#include "filesystem.h"
#include "utils.h"

/**
 * get_ext_common
 */
void get_ext_common (Filesystem *f, unsigned char *buffer, int bufsize)
{
	if (!f)
		return;
	if (!buffer)
		return;

	f->uuid = read_uuid1 (buffer + 0x468);
	f->label = (char*) (buffer+0x478);

	int block_size = *(int *) (buffer + 0x418);
	block_size = (1 << (block_size+10));		// 2^(10+ value at 0x18)

	long blocks_total = *(int *) (buffer + 0x404);
	long blocks_free  = *(int *) (buffer + 0x40C);

	f->block_size = block_size;
	f->bytes_size = blocks_total * block_size;
	f->bytes_used = (blocks_total - blocks_free) * block_size;

#if 0
	log_info ("%s\n", f->name.c_str());
	log_info ("\tblock = %d\n", block_size);
	log_info ("\tsize = %ld\n",      f->bytes_size);
	log_info ("\tused = %ld (%ld)\n", f->bytes_used, (f->bytes_used*100/f->bytes_size));
#endif
}


/**
 * get_btrfs
 */
Filesystem *
get_btrfs (unsigned char *buffer, int bufsize)
{
	Filesystem *f = NULL;

	if (!identify_btrfs (buffer, bufsize))
		return NULL;

	//log_info ("bufsize = %d, want %d\n", bufsize, 0x10140);
	f = new Filesystem();

	f->name = "btrfs";
	f->uuid = read_uuid1 (buffer + 0x10020);
	f->label = (char*) (buffer+0x1012B);

	f->bytes_size = *(long*) (buffer + 0x10070);
	f->bytes_used = *(long*) (buffer + 0x10078);

	get_btrfs_usage (f);
	return f;
}

/**
 * get_ext2
 */
Filesystem *
get_ext2 (unsigned char *buffer, int bufsize)
{
	Filesystem *f = NULL;

	if (!identify_ext2 (buffer, bufsize))
		return NULL;

	f = new Filesystem();
	f->name = "ext2";

	get_ext_common (f, buffer, bufsize);

	get_ext2_usage (f);
	return f;
}

/**
 * get_ext3
 */
Filesystem *
get_ext3 (unsigned char *buffer, int bufsize)
{
	Filesystem *f = NULL;

	if (!identify_ext3 (buffer, bufsize))
		return NULL;

	f = new Filesystem();
	f->name = "ext3";

	get_ext_common (f, buffer, bufsize);

	get_ext3_usage (f);
	return f;
}

/**
 * get_ext4
 */
Filesystem *
get_ext4 (unsigned char *buffer, int bufsize)
{
	Filesystem *f = NULL;

	if (!identify_ext4 (buffer, bufsize))
		return NULL;

	f = new Filesystem();
	f->name = "ext4";

	get_ext_common (f, buffer, bufsize);

	get_ext4_usage (f);
	return f;
}

/**
 * get_ntfs
 */
Filesystem *
get_ntfs (unsigned char *buffer, int bufsize)
{
	Filesystem *f = NULL;

	if (!identify_ntfs (buffer, bufsize))
		return NULL;

	std::string uuid = read_uuid2 (buffer + 72);
	long size = *(long*) (buffer + 40) * 512;

	f = new Filesystem();

	f->name = "ntfs";
	f->uuid = uuid;
	f->label = "";
	f->bytes_size = size;

	get_ntfs_usage (f);
	return f;
}

/**
 * get_reiserfs
 */
Filesystem *
get_reiserfs (unsigned char *buffer, int bufsize)
{
	Filesystem *f = NULL;

	if (!identify_reiserfs (buffer, bufsize))
		return NULL;

	f = new Filesystem();

	f->name = "reiserfs";
	f->uuid = read_uuid1 (buffer + 0x10054);
	f->label = (char*) (buffer+0x10064);

	short    int block_size   = *(short    int *) (buffer + 0x1002C);
	unsigned int blocks_total = *(unsigned int *) (buffer + 0x10000);
	unsigned int blocks_free  = *(unsigned int *) (buffer + 0x10004);

	f->bytes_size = (blocks_total * block_size);
	f->bytes_used = (blocks_total - blocks_free) * block_size;

	//log_info ("reiser: %s, %s, %ld, %ld\n", f->uuid.c_str(), f->label.c_str(), f->bytes_size, f->bytes_used);

	get_reiserfs_usage (f);
	return f;
}

/**
 * get_swap
 */
Filesystem *
get_swap (unsigned char *buffer, int bufsize)
{
	Filesystem *f = NULL;

	if (!identify_swap (buffer, bufsize))
		return NULL;

	std::string uuid     = read_uuid1 (buffer + 0x40C);
	std::string vol_name = (char*) (buffer+0x41C);
	long size = 0;

	if (!strncmp ((char*) buffer+4086, "SWAPSPACE2", 10)) {
		// implies 4K pages
		size = *(long*) (buffer + 0x404) * 4096;
	}

	f = new Filesystem();

	f->name = "swap";
	f->uuid = uuid;
	f->label = vol_name;
	f->bytes_size = size;

	get_swap_usage (f);
	return f;
}

/**
 * get_vfat
 */
Filesystem *
get_vfat (unsigned char *buffer, int bufsize)
{
	Filesystem *f = NULL;

	if (!identify_vfat (buffer, bufsize))
		return NULL;

	f = new Filesystem();

	f->name = "vfat";
	f->uuid = read_uuid3 (buffer+0x1C);
	f->label = std::string ((char*)(buffer+0x30), 14);

	long sectors = *(short int*) (buffer + 0x13);
	if (sectors == 0) {
		sectors = *(int*) (buffer + 0x20);
	}

	f->bytes_size = sectors * 512;

#if 1
	// will likely exceed bufsize
	int reserved   = *(short int*) (buffer + 0x0E);
	int sect_fat   = *(short int*) (buffer + 0x16);
	int sect_clust = buffer[0x0D];
	int clusters   = sectors / sect_clust;
	int free_clust = 0;

	if ((sect_fat * 512) <= bufsize) {
		short int *ptr = (short int*) (buffer + (512*reserved));
		if (sect_fat == 0) {
			sect_fat = *(int*) (buffer + 0x24);
		}

		for (int i = 0; i < clusters; i++) {
			if (ptr[i] == 0) {
				free_clust++;
			}
		}

		f->bytes_used = f->bytes_size - (free_clust * 512 * sect_clust);
		//log_info ("size = %ld, used = %ld\n", f->bytes_size, f->bytes_used);
	}

	//log_info ("res = %d, sect/fat = %d\n", reserved, sect_fat);
#endif

	get_vfat_usage (f);
	return f;
}

/**
 * get_xfs
 */
Filesystem *
get_xfs (unsigned char *buffer, int bufsize)
{
	Filesystem *f = NULL;

	if (!identify_xfs (buffer, bufsize))
		return NULL;

	f = new Filesystem();

	f->name = "xfs";
	f->uuid = read_uuid1 (buffer + 0x20);
	f->label = (char*) (buffer+0x6C);

	int  block_size   = *(int  *) (buffer + 0x04);
	long blocks_total = *(long *) (buffer + 0x08);
	long blocks_free  = *(long *) (buffer + 0x90);

	block_size   = be32toh (block_size);
	blocks_total = be64toh (blocks_total);
	blocks_free  = be64toh (blocks_free);

	f->bytes_size = blocks_total * block_size;
	f->bytes_used = (blocks_total - blocks_free) * block_size;

	get_xfs_usage (f);
	return f;
}


