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
#include "visitor.h"
#include "log.h"

LvmVolume::LvmVolume (void)
{
	LOG_CTOR;
	const char* me = "LvmVolume";

	sub_type (me);

	declare_prop_var (me, "lv_attr",      lv_attr,      "desc of lv_attr",      0);
	declare_prop_var (me, "seg_count",    seg_count,    "desc of seg_count",    0);
	declare_prop_var (me, "seg_start_pe", seg_start_pe, "desc of seg_start_pe", 0);
	declare_prop_var (me, "stripes",      stripes,      "desc of stripes",      0);
	declare_prop_var (me, "stripe_size",  stripe_size,  "desc of stripe_size",  0);
}

LvmVolume::LvmVolume (const LvmVolume& c) :
	Volume(c)
{
	LvmVolume();
	LOG_CTOR;
	lv_attr      = c.lv_attr;
	seg_count    = c.seg_count;
	seg_start_pe = c.seg_start_pe;
	stripes      = c.stripes;
	stripe_size  = c.stripe_size;
	mirror_log   = c.mirror_log;
	metadata     = c.metadata;
	subvols      = c.subvols;
	sibling      = c.sibling;
}

LvmVolume::LvmVolume (LvmVolume&& c)
{
	LOG_CTOR;
	swap(c);
}

LvmVolume::~LvmVolume()
{
	LOG_DTOR;
}

LvmVolumePtr
LvmVolume::create (void)
{
	LvmVolumePtr p (new LvmVolume());
	p->self = p;

	return p;
}


LvmVolume&
LvmVolume::operator= (const LvmVolume& c)
{
	lv_attr      = c.lv_attr;
	seg_count    = c.seg_count;
	seg_start_pe = c.seg_start_pe;
	stripes      = c.stripes;
	stripe_size  = c.stripe_size;
	mirror_log   = c.mirror_log;
	metadata     = c.metadata;
	subvols      = c.subvols;
	sibling      = c.sibling;

	return *this;
}

LvmVolume&
LvmVolume::operator= (LvmVolume&& c)
{
	swap(c);
	return *this;
}


void
LvmVolume::swap (LvmVolume& c)
{
	std::swap (lv_attr,      c.lv_attr);
	std::swap (seg_count,    c.seg_count);
	std::swap (seg_start_pe, c.seg_start_pe);
	std::swap (stripes,      c.stripes);
	std::swap (stripe_size,  c.stripe_size);
	std::swap (mirror_log,   c.mirror_log);
	std::swap (metadata,     c.metadata);
	std::swap (subvols,      c.subvols);
	std::swap (sibling,      c.sibling);
}

void
swap (LvmVolume& lhs, LvmVolume& rhs)
{
	lhs.swap (rhs);
}


LvmVolume*
LvmVolume::clone (void)
{
	LOG_TRACE;
	return new LvmVolume (*this);
}


bool
LvmVolume::accept (Visitor& v)
{
	LvmVolumePtr l = std::dynamic_pointer_cast<LvmVolume> (get_smart());
	if (!v.visit(l))
		return false;

	if (!visit_children(v)) {
		log_debug ("LvmVolume visit_children failed");
		return false;
	}

	for (auto& i : metadata) {
		LvmLinearPtr meta = std::dynamic_pointer_cast<LvmLinear>(i);
		if (!v.visit(meta)) {
			log_debug ("LvmVolume metadata failed");
			return false;
		}
	}

	for (auto& i : subvols) {
		//XXX need most-derived type, really
		LvmVolumePtr vol = std::dynamic_pointer_cast<LvmVolume>(i);
		if (!v.visit(vol)) {
			log_debug ("LvmVolume metadata failed");
			return false;
		}
	}

	return true;
}


std::vector<Action>
LvmVolume::get_actions (void)
{
	LOG_TRACE;

	ContainerPtr me = get_smart();
	std::vector<Action> actions = {
		{ "dummy.lvm_volume", "Dummy/Lvm Volume", me, true },
	};

	std::vector<Action> base_actions = Volume::get_actions();

	actions.insert (std::end (actions), std::begin (base_actions), std::end (base_actions));

	return actions;
}

bool
LvmVolume::perform_action (Action action)
{
	if (action.name == "dummy.lvm_volume") {
		log_debug ("LvmVolume perform: %s", SP(action.name));
		return true;
	} else {
		return Volume::perform_action (action);
	}
}


bool
LvmVolume::add_child (ContainerPtr child, bool probe)
{
	return_val_if_fail (child, false);

	if ((is_a ("LvmMetadata") && (child->is_a ("LvmVolume")))) {	//XXX tmp
		log_debug ("LvmMetadata: %s", SP(child->get_type()));
		sibling = child;
		LvmVolumePtr vol = std::dynamic_pointer_cast<LvmVolume> (child);
		vol->sibling = get_smart();
		return true;
	}

	log_debug ("volume: %s (%s), child: %s (%s)", SP(name), SP(get_type()), SP(child->name), SP(child->get_type()));
	if (child->is_a ("LvmMetadata")) {
		_add_child (metadata, child);
		log_debug ("metadata: %s (%s) -- %s", SP(this->name), SP(child->name), SP(child->uuid));
		log_info ("METADATA %s", SP(child->name));
		//XXX no this is self-contained child->whole = this;
	} else if (child->is_a ("LvmVolume")) {
		_add_child (subvols, child);
		log_debug ("subvols: %s (%s) -- %s", SP(this->name), SP(child->name), SP(child->uuid));
		log_info ("SUBVOL %s", SP(child->name));
		child->whole = get_smart();
	} else if (child->is_a ("LvmPartition")) {
		log_info ("PARTITION %s", SP(child->name));
		add_segment (child);
		// Volume::add_child (child);
		child->whole = get_smart();
	} else if (child->is_a ("Space")) {
		log_info ("SPACE %s", SP(child->name));
		return Volume::add_child (child, false);
	} else {
		// filesystem
		return Volume::add_child (child, probe);

#if 0
		for (auto& i : subvols) {
			LvmVolumePtr v = std::dynamic_pointer_cast<LvmVolume>(i);
			log_info ("subvol %s, %ld segments", SP(v->name), v->segments.size());
			for (auto& j : v->segments) {
				j->add_child (child, false);
			}
		}
#endif
	}

	return true;
}

ContainerPtr
LvmVolume::find (const std::string& search)
{
	log_debug ("find: %s -> %s (%s)", SP(search), SP(name), SP(uuid));
	std::string search2;

	std::size_t pos = search.find ("(0)");
	if (pos == (search.length() - 3)) {
		search2 = search.substr (0, pos);
	}

	for (auto& i : metadata) {
		if (i->uuid == search) {
			log_info ("metadata uuid %s", SP(i->uuid));
			return i;
		}
		if (i->name == search) {
			log_info ("metadata name %s", SP(i->uuid));
			return i;
		}
		if (i->name == search2) {
			log_info ("metadata name2 %s", SP(i->uuid));
			return i;
		}
	}

	for (auto& i : subvols) {
		if (i->uuid == search) {
			log_info ("subvols uuid %s", SP(i->uuid));
			return i;
		}
		if (i->name == search) {
			log_info ("subvols name %s", SP(i->uuid));
			return i;
		}
		if (i->name == search2) {
			log_info ("subvols name2 %s", SP(i->uuid));
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


