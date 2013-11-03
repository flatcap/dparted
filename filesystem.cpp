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

#include <cstdlib>
#include <sstream>
#include <string>

#include "filesystem.h"
#include "fs_identify.h"
#include "fs_get.h"
#include "log.h"
#include "partition.h"
#include "stringnum.h"
#include "utils.h"
#include "log_trace.h"

/**
 * Filesystem
 */
Filesystem::Filesystem (void)
{
	declare ("filesystem");
}

/**
 * ~Filesystem
 */
Filesystem::~Filesystem()
{
}


/**
 * probe
 */
DPContainer *
Filesystem::probe (DPContainer &top_level, DPContainer *parent)
{
	//LOG_TRACE;

	if (!parent)
		return nullptr;

	long		 bufsize = 131072;	// 128 KiB, enough for the fs signatures
	unsigned char	*buffer  = parent->get_buffer (0, bufsize);

	if (!buffer) {
		log_error ("can't get buffer\n");
		return nullptr;
	}

	Filesystem *f = nullptr;
	//XXX reorder by likelihood
	     if ((f = get_btrfs    (buffer, bufsize))) {}
	else if ((f = get_ext2     (buffer, bufsize))) {}
	else if ((f = get_ext3     (buffer, bufsize))) {}
	else if ((f = get_ext4     (buffer, bufsize))) {}
	else if ((f = get_ntfs     (buffer, bufsize))) {}
	else if ((f = get_reiserfs (buffer, bufsize))) {}
	else if ((f = get_swap     (buffer, bufsize))) {}
	else if ((f = get_vfat     (buffer, bufsize))) {}
	else if ((f = get_xfs      (buffer, bufsize))) {}

	if (f) {
		//log_info ("volume: %s (%s), child: %s\n", parent->name.c_str(), parent->type.back().c_str(), f->name.c_str());
		parent->add_child (f);
	}

	return f;
}


