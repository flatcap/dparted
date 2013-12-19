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

#include "lvm_metadata.h"
#include "log_trace.h"

/**
 * create
 */
LvmMetadataPtr
LvmMetadata::create (void)
{
	LvmMetadataPtr l (new LvmMetadata());
	l->declare ("lvm_metadata");
	l->weak = l;

	return l;
}


/**
 * add_child
 */
void
LvmMetadata::add_child (ContainerPtr& child)
{
	if (!child)
		return;

	if (child->is_a ("lvm_volume")) {
		LvmVolumePtr v = std::dynamic_pointer_cast<LvmVolume> (child);
		sibling = v;
		v->sibling.reset (this);
	} else {
		// probably a partition
		add_segment (child);
		//log_debug ("segment: %s (%s) -- %s\n", this->name.c_str(), child->name.c_str(), child->uuid.c_str());
	}
}


