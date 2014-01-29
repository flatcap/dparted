/* Copyright (c) 2014 Richard Russon (FlatCap)
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
#include "log_trace.h"

#if 0
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
get_mounted_usage (FilesystemPtr f)
{
	if (!f)
		return false;
	if (f->get_device_name().empty())
		return false;

	return false;
}


/**
 * get_ntfs_usage
 */
bool
get_ntfs_usage (FilesystemPtr f)
{
	if (!f)
		return false;
	if (f->device.empty())
		return false;
	if (get_mounted_usage(f))
		return true;

	return false;
}

/**
 * get_reiserfs_usage
 */
bool
get_reiserfs_usage (FilesystemPtr f)
{
	if (!f)
		return false;
	if (f->device.empty())
		return false;
	if (get_mounted_usage(f))
		return true;

	return false;
}

/**
 * get_swap_usage
 */
bool
get_swap_usage (FilesystemPtr f)
{
	if (!f)
		return false;
	if (f->device.empty())
		return false;
	if (get_mounted_usage(f))
		return true;

	return false;
}

/**
 * get_vfat_usage
 */
bool
get_vfat_usage (FilesystemPtr f)
{
	if (!f)
		return false;
	if (f->device.empty())
		return false;
	if (get_mounted_usage(f))
		return true;

	return false;
}

/**
 * get_xfs_usage
 */
bool
get_xfs_usage (FilesystemPtr f)
{
	if (!f)
		return false;
	if (f->device.empty())
		return false;
	if (get_mounted_usage(f))
		return true;

	return false;
}


