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

#include "filesystem.h"
#include "log_trace.h"

#if 0
reiserfs	reiserfstune {device}
swap		swapon -s (if mounted, else 0)
vfat		dosfsck -n {device}
xfs		xfs_db {device}, sb, p
#endif

bool
get_mounted_usage (FilesystemPtr f)
{
	if (!f)
		return false;
	if (f->get_device_name().empty())
		return false;

	return false;
}


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


