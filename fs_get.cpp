/* Copyright (c) 2014 Richard Russon (FlatCap)
 *
 * This file is part of DParted.
 *
 * DParted is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DParted is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DParted.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string>
#include <cstring>

#include "endian.h"
#include "filesystem.h"
#include "fs_identify.h"
#include "fs_usage.h"
#include "log_trace.h"
#include "utils.h"

FilesystemPtr
get_reiserfs (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize)
{
	//LOG_TRACE;

	if (!parent || !buffer || !bufsize)
		return nullptr;

	if (!identify_reiserfs (buffer, bufsize))
		return nullptr;

	FilesystemPtr f = Filesystem::create();
	f->sub_type ("reiserfs");

	f->name = get_fixed_str (buffer+0x10064, 16);
	f->uuid = read_uuid1 (buffer + 0x10054);

	short    int block_size   = le16_to_cpup (buffer + 0x1002C);
	unsigned int blocks_total = le32_to_cpup (buffer + 0x10000);
	unsigned int blocks_free  = le32_to_cpup (buffer + 0x10004);

	f->block_size = block_size;
	f->bytes_size = (blocks_total * block_size);
	f->bytes_used = (blocks_total - blocks_free) * block_size;

	//log_info ("reiser: %s, %s, %ld, %ld\n", f->uuid.c_str(), f->name.c_str(), f->bytes_size, f->bytes_used);

	get_reiserfs_usage(f);
	return f;
}

FilesystemPtr
get_swap (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize)
{
	//LOG_TRACE;

	if (!parent || !buffer || !bufsize)
		return nullptr;

	if (!identify_swap (buffer, bufsize))
		return nullptr;

	std::string uuid     = read_uuid1 (buffer + 0x40C);
	std::string vol_name = get_fixed_str (buffer+0x41C, 16);
	std::uint64_t size = 0;
	std::uint64_t block = 0;

	if (!strncmp ((char*) buffer+4086, "SWAPSPACE2", 10)) {
		block = 4096;
		size  = le64_to_cpup (buffer + 0x404) * block;
	}

	FilesystemPtr f  = Filesystem::create();
	f->sub_type ("swap");

	f->name = vol_name;
	f->uuid = uuid;
	f->bytes_size = size;
	f->block_size = block;

	get_swap_usage(f);
	return f;
}

FilesystemPtr
get_vfat (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize)
{
	//LOG_TRACE;

	if (!parent || !buffer || !bufsize)
		return nullptr;

	if (!identify_vfat (buffer, bufsize))
		return nullptr;

	FilesystemPtr f  = Filesystem::create();
	f->sub_type ("vfat");

	f->name = std::string ((char*) (buffer+0x30), 14);
	if (!f->name.empty() && ((f->name[0] < 'A') || (f->name[0] > 'z')))
		f->name.clear();	//XXX this isn't a reliable place to find the label

	f->uuid = read_uuid3 (buffer+0x1C);

	std::uint64_t sectors = le32_to_cpup (buffer + 0x13);
	if (sectors == 0) {
		sectors = le32_to_cpup (buffer + 0x20);
	}

	f->bytes_size = sectors * 512;

#if 1
	// will likely exceed bufsize
	std::uint32_t reserved   = le16_to_cpup (buffer + 0x0E);
	std::uint32_t sect_fat   = le16_to_cpup (buffer + 0x16);
	std::uint32_t sect_clust = buffer[0x0D];
	std::uint32_t clusters   = sectors / sect_clust;
	std::uint32_t free_clust = 0;

	f->block_size = sect_clust * 512;

	if ((sect_fat * 512) <= bufsize) {
		std::uint16_t* ptr = (std::uint16_t*) (buffer + (512*reserved));
#if 0
		//XXX not stored anywhere
		if (sect_fat == 0) {
			sect_fat = le32_to_cpup (buffer + 0x24);
		}
#endif

		for (std::uint32_t i = 0; i < clusters; ++i) {
			if (le32_to_cpu (ptr[i]) == 0) {
				++free_clust;
			}
		}

		f->bytes_used = f->bytes_size - (free_clust * 512 * sect_clust);
		//log_info ("size = %ld, used = %ld\n", f->bytes_size, f->bytes_used);
	}

	//log_info ("res = %d, sect/fat = %d\n", reserved, sect_fat);
#endif

	get_vfat_usage(f);
	return f;
}

FilesystemPtr
get_xfs (ContainerPtr& parent, std::uint8_t* buffer, std::uint64_t bufsize)
{
	//LOG_TRACE;

	if (!parent || !buffer || !bufsize)
		return nullptr;

	if (!identify_xfs (buffer, bufsize))
		return nullptr;

	FilesystemPtr f  = Filesystem::create();
	f->sub_type ("xfs");

	f->name = get_fixed_str (buffer+0x6C, 12);
	f->uuid = read_uuid1 (buffer + 0x20);

	std::uint32_t block_size   = le32_to_cpup (buffer + 0x04);
	std::uint64_t blocks_total = le64_to_cpup (buffer + 0x08);
	std::uint64_t blocks_free  = le64_to_cpup (buffer + 0x90);

	block_size   = be32toh (block_size);
	blocks_total = be64toh (blocks_total);
	blocks_free  = be64toh (blocks_free);

	f->block_size = block_size;
	f->bytes_size = blocks_total * block_size;
	f->bytes_used = (blocks_total - blocks_free) * block_size;

	get_xfs_usage(f);
	return f;
}


