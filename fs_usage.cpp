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

#include "filesystem.h"

#if 0
/**
 * ext2_get_usage
 */
long
Filesystem::ext2_get_usage (void)
{
	std::ostringstream build;
	std::string dev = device;
	std::string command;
	std::string output;
	long block_size = 512;
	long bfree = 0;

	if (parent_offset != 0) {
		//log_debug ("create loop device\n");
		build << "losetup /dev/loop16 " << device << " -o " << parent_offset;
		command = build.str();
		execute_command3 (command, output);
		//log_debug ("command = %s\n", command.c_str());
		//execute_command ("losetup /dev/loop16 ", output);
		dev = "/dev/loop16";
	}

	if (dev.empty())	//XXX shouldn't happen
		return 0;

	// do something
	command = "tune2fs -l " + dev;
	//log_debug ("command = %s\n", command.c_str());

	execute_command3 (command, output);
	//log_debug ("result = \n%s\n", output.c_str());

	//interpret results
	std::string tmp;
	size_t pos1 = std::string::npos;
	size_t pos2 = std::string::npos;

	//log_debug ("volume:\n");

	pos1 = output.find ("Filesystem volume name:");
	if (pos1 != std::string::npos) {
		pos1 = output.find_first_not_of (" ", pos1 + 23);
		pos2 = output.find_first_of ("\n\r", pos1);

		tmp = output.substr (pos1, pos2 - pos1);
		//log_debug ("\tname = '%s'\n", tmp.c_str());
	}

	pos1 = output.find ("Filesystem UUID:");
	if (pos1 != std::string::npos) {
		pos1 = output.find_first_not_of (" ", pos1 + 16);
		pos2 = output.find_first_of ("\n\r", pos1);

		tmp = output.substr (pos1, pos2 - pos1);
		//log_debug ("\tuuid = '%s'\n", tmp.c_str());
	}

	pos1 = output.find ("Block size:");
	if (pos1 != std::string::npos) {
		pos1 = output.find_first_not_of (" ", pos1 + 11);
		pos2 = output.find_first_of ("\n\r", pos1);

		tmp = output.substr (pos1, pos2 - pos1);
		StringNum s (tmp.c_str());
		block_size = s;
		//log_debug ("\tblock size = %ld\n", block_size);
	}

	pos1 = output.find ("Block count:");
	if (pos1 != std::string::npos) {
		pos1 = output.find_first_not_of (" ", pos1 + 12);
		pos2 = output.find_first_of ("\n\r", pos1);

		tmp = output.substr (pos1, pos2 - pos1);
		//log_debug ("\tblock count = %s\n", tmp.c_str());
	}

	pos1 = output.find ("Free blocks:");
	if (pos1 != std::string::npos) {
		pos1 = output.find_first_not_of (" ", pos1 + 12);
		pos2 = output.find_first_of ("\n\r", pos1);

		tmp = output.substr (pos1, pos2 - pos1);
		StringNum s (tmp.c_str());
		bfree = (long) s * block_size;
		//log_debug ("\tfree blocks = %s\n", tmp.c_str());
		//log_debug ("\tbytes free = %lld\n", bfree);
	}

	if (parent_offset != 0) {
		command = "losetup -d /dev/loop16";
		execute_command3 (command, output);
		//log_debug ("dismantle loop device\n");
		//log_debug ("command = %s\n", command.c_str());
		//log_debug ("\n");
	}

	return bfree;
}

#endif

#if 0
ext2,3,4	tune2fs -l {device}
btrfs		btrfs filesystem show {uuid}
ntfs		ntfsinfo -m {device}
swap		swapon -s (if mounted, else 0)
reiserfs	reiserfstune {device}
vfat		dosfsck -n {device}
xfs		xfs_db {device}, sb, p
#endif

/**
 * get_mounted_usage
 */
bool
get_mounted_usage (Filesystem *f)
{
	if (!f)
		return false;
	if (f->device.empty())
		return false;

	return false;
}

/**
 * get_btrfs_usage
 */
bool
get_btrfs_usage (Filesystem *f)
{
	if (!f)
		return false;
	if (f->device.empty())
		return false;
	if (get_mounted_usage (f))
		return true;

	return false;
}

/**
 * get_ext2_usage
 */
bool
get_ext2_usage (Filesystem *f)
{
	if (!f)
		return false;
	if (f->device.empty())
		return false;
	if (get_mounted_usage (f))
		return true;

	return false;
}

/**
 * get_ext3_usage
 */
bool
get_ext3_usage (Filesystem *f)
{
	if (!f)
		return false;
	if (f->device.empty())
		return false;
	if (get_mounted_usage (f))
		return true;

	return false;
}

/**
 * get_ext4_usage
 */
bool
get_ext4_usage (Filesystem *f)
{
	if (!f)
		return false;
	if (f->device.empty())
		return false;
	if (get_mounted_usage (f))
		return true;

	return false;
}

/**
 * get_ntfs_usage
 */
bool
get_ntfs_usage (Filesystem *f)
{
	if (!f)
		return false;
	if (f->device.empty())
		return false;
	if (get_mounted_usage (f))
		return true;

	return false;
}

/**
 * get_reiserfs_usage
 */
bool
get_reiserfs_usage (Filesystem *f)
{
	if (!f)
		return false;
	if (f->device.empty())
		return false;
	if (get_mounted_usage (f))
		return true;

	return false;
}

/**
 * get_swap_usage
 */
bool
get_swap_usage (Filesystem *f)
{
	if (!f)
		return false;
	if (f->device.empty())
		return false;
	if (get_mounted_usage (f))
		return true;

	return false;
}

/**
 * get_vfat_usage
 */
bool
get_vfat_usage (Filesystem *f)
{
	if (!f)
		return false;
	if (f->device.empty())
		return false;
	if (get_mounted_usage (f))
		return true;

	return false;
}

/**
 * get_xfs_usage
 */
bool
get_xfs_usage (Filesystem *f)
{
	if (!f)
		return false;
	if (f->device.empty())
		return false;
	if (get_mounted_usage (f))
		return true;

	return false;
}


