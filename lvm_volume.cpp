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

#include <sstream>

#include "lvm_volume.h"
#include "log_trace.h"

/**
 * create
 */
LvmVolumePtr
LvmVolume::create (void)
{
	LvmVolumePtr l (new LvmVolume());
	l->declare ("lvm_volume");
	l->weak = l;

	return l;
}


/**
 * add_child
 */
void
LvmVolume::add_child (ContainerPtr& child)
{
	if (!child)
		return;

	//log_info ("volume: %s (%s), child: %s (%s)\n", name.c_str(), type.back().c_str(), child->name.c_str(), child->type.back().c_str());
	if (child->is_a ("lvm_metadata")) {
		metadata.push_back (child);
		//log_debug ("metadata: %s (%s) -- %s\n", this->name.c_str(), child->name.c_str(), child->uuid.c_str());
		//log_info ("METADATA %s\n", child->name.c_str());
		//XXX no this is self-contained child->whole = this;
	} else if (child->is_a ("lvm_volume")) {
		subvols.push_back (child);
		//log_debug ("subvols: %s (%s) -- %s\n", this->name.c_str(), child->name.c_str(), child->uuid.c_str());
		//log_info ("SUBVOL %s\n", child->name.c_str());
		child->whole.reset (this);
	} else if (child->is_a ("lvm_partition")) {
		//log_info ("PARTITION %s\n", child->name.c_str());
		add_segment (child);
		//Whole::add_child (child);
		child->whole.reset (this);
	} else {
		// filesystem
		Whole::add_child (child);

#if 0
		for (auto i : subvols) {
			LvmVolumePtr v = std::dynamic_pointer_cast<LvmVolume> (i);
			//log_info ("subvol %s, %ld segments\n", v->name.c_str(), v->segments.size());
			for (auto j : v->segments) {
				j->just_add_child (child);
			}
		}
#endif
	}
}

/**
 * find
 */
ContainerPtr
LvmVolume::find (const std::string& search)
{
	//printf ("find: %s -> %s (%s)\n", search.c_str(), name.c_str(), uuid.c_str());
	std::string search2;

	size_t pos = search.find ("(0)");
	if (pos == (search.length() - 3)) {
		search2 = search.substr (0, pos);
	}

	for (auto i : metadata) {
		if (i->uuid == search) {
			//log_info ("metadata uuid %s\n", i->uuid.c_str());
			return i;
		}
		if (i->name == search) {
			//log_info ("metadata name %s\n", i->uuid.c_str());
			return i;
		}
		if (i->name == search2) {
			//log_info ("metadata name2 %s\n", i->uuid.c_str());
			return i;
		}
	}

	for (auto i : subvols) {
		if (i->uuid == search) {
			//log_info ("subvols uuid %s\n", i->uuid.c_str());
			return i;
		}
		if (i->name == search) {
			//log_info ("subvols name %s\n", i->uuid.c_str());
			return i;
		}
		if (i->name == search2) {
			//log_info ("subvols name2 %s\n", i->uuid.c_str());
			return i;
		}
	}

	if (sibling) {
		if (sibling->uuid == search)
			return sibling;
		if (sibling->name == search)
			return sibling;
		if (sibling->name == search2)
			return sibling;
	}

	return Whole::find (search);
}


/**
 * dump_objects
 */
void
LvmVolume::dump_objects (int indent)
{
	printf ("%*s", 8*indent, "");
	if (name == "dummy") {
		indent--;
	} else {
		std::cout << this << std::endl;
	}

	for (auto c : subvols) {
		c->dump_objects (indent+1);
	}

	for (auto c : metadata) {
		c->dump_objects (indent+1);
	}

	for (auto c : children) {
		c->dump_objects (indent+1);
	}
}


