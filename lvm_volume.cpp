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

#include <sstream>

#include "lvm_volume.h"
#include "action.h"
#include "log_trace.h"
#include "visitor.h"

LvmVolume::LvmVolume (void)
{
	const char* me = "LvmVolume";

	sub_type (me);

	declare_prop_var (me, "lv_attr",      lv_attr,      "desc of lv_attr",      0);
	declare_prop_var (me, "seg_count",    seg_count,    "desc of seg_count",    0);
	declare_prop_var (me, "seg_start_pe", seg_start_pe, "desc of seg_start_pe", 0);
	declare_prop_var (me, "stripes",      stripes,      "desc of stripes",      0);
	declare_prop_var (me, "stripe_size",  stripe_size,  "desc of stripe_size",  0);
}

LvmVolume::~LvmVolume()
{
}

LvmVolumePtr
LvmVolume::create (void)
{
	LvmVolumePtr p (new LvmVolume());
	p->weak = p;

	return p;
}


bool
LvmVolume::accept (Visitor& v)
{
	LvmVolumePtr l = std::dynamic_pointer_cast<LvmVolume> (get_smart());
	if (!v.visit(l))
		return false;

	if (!visit_children(v)) {
		std::cout << "LvmVolume visit_children failed" << std::endl;
		return false;
	}

	for (auto i : metadata) {
		LvmLinearPtr meta = std::dynamic_pointer_cast<LvmLinear>(i);
		if (!v.visit(meta)) {
			std::cout << "LvmVolume metadata failed" << std::endl;
			return false;
		}
	}

	for (auto i : subvols) {
		//XXX need most-derived type, really
		LvmVolumePtr vol = std::dynamic_pointer_cast<LvmVolume>(i);
		if (!v.visit(vol)) {
			std::cout << "LvmVolume metadata failed" << std::endl;
			return false;
		}
	}

	return true;
}


std::vector<Action>
LvmVolume::get_actions (void)
{
	// LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.lvm_volume", true },
	};

	std::vector<Action> parent_actions = Volume::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
LvmVolume::perform_action (Action action)
{
	if (action.name == "dummy.lvm_volume") {
		std::cout << "LvmVolume perform: " << action.name << std::endl;
		return true;
	} else {
		return Volume::perform_action (action);
	}
}


void
LvmVolume::add_child (ContainerPtr& child)
{
	if (!child)
		return;

	if ((is_a ("LvmMetadata") && (child->is_a ("LvmVolume")))) {	//XXX tmp
		//printf ("LvmMetadata: %s\n", child->type.back().c_str());
		sibling = child;
		LvmVolumePtr vol = std::dynamic_pointer_cast<LvmVolume>(child);
		vol->sibling = get_smart();
		return;
	}

	//log_info ("volume: %s (%s), child: %s (%s)\n", name.c_str(), type.back().c_str(), child->name.c_str(), child->type.back().c_str());
	if (child->is_a ("LvmMetadata")) {
		metadata.insert (child);
		//log_debug ("metadata: %s (%s) -- %s\n", this->name.c_str(), child->name.c_str(), child->uuid.c_str());
		//log_info ("METADATA %s\n", child->name.c_str());
		//XXX no this is self-contained child->whole = this;
	} else if (child->is_a ("LvmVolume")) {
		subvols.insert (child);
		//log_debug ("subvols: %s (%s) -- %s\n", this->name.c_str(), child->name.c_str(), child->uuid.c_str());
		//log_info ("SUBVOL %s\n", child->name.c_str());
		child->whole = get_smart();
	} else if (child->is_a ("LvmPartition")) {
		//log_info ("PARTITION %s\n", child->name.c_str());
		add_segment (child);
		//Volume::add_child (child);
		child->whole = get_smart();
	} else if (child->is_a ("Space")) {
		//log_info ("SPACE %s\n", child->name.c_str());
		Volume::add_child (child);
	} else {
		// filesystem
		Volume::add_child (child);

#if 0
		for (auto i : subvols) {
			LvmVolumePtr v = std::dynamic_pointer_cast<LvmVolume>(i);
			//log_info ("subvol %s, %ld segments\n", v->name.c_str(), v->segments.size());
			for (auto j : v->segments) {
				j->just_add_child (child);
			}
		}
#endif
	}
}

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

	return Volume::find (search);
}


