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

#include <iterator>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <algorithm>

#include "lvm_group.h"
#include "action.h"
#include "app.h"
#include "log.h"
#include "log_trace.h"
#include "lvm_linear.h"
#include "lvm_mirror.h"
#include "lvm_partition.h"
#include "lvm_raid.h"
#include "lvm_stripe.h"
#include "lvm_table.h"
#include "lvm_volume.h"
#include "main.h"
#include "type_visitor.h"
#include "utils.h"
#include "uuid_visitor.h"
#include "visitor.h"

LvmGroup::LvmGroup (void)
{
	log_ctor ("ctor LvmGroup");
	const char* me = "LvmGroup";

	sub_type (me);

	declare_prop_var (me, "lv_count", lv_count, "desc of lv_count", 0);
	declare_prop_var (me, "pv_count", pv_count, "desc of pv_count", 0);
	declare_prop_var (me, "vg_attr",  vg_attr,  "desc of vg_attr",  0);
	declare_prop_var (me, "vg_seqno", vg_seqno, "desc of vg_seqno", 0);
}

LvmGroup::~LvmGroup()
{
	log_dtor ("dtor LvmGroup");
}

LvmGroupPtr
LvmGroup::create (void)
{
	LvmGroupPtr p (new LvmGroup());
	p->self = p;

	return p;
}


bool
LvmGroup::accept (Visitor& v)
{
	LvmGroupPtr l = std::dynamic_pointer_cast<LvmGroup> (get_smart());
	if (!v.visit(l))
		return false;

	return visit_children(v);
}


void
expand_name (std::string& name)
{
	std::size_t pos = 0;

	while ((pos = name.find_first_of ("-", pos)) != std::string::npos) {
		name.insert (pos, "-");
		pos += 2;
	}
}

std::string
make_device (std::string group, std::string volume)
{
	// Either:
	//	/dev/GROUP/VOLUME
	//	/dev/mapper/GROUP-VOLUME(*)
	// (*) with - replace with -- in both parts
	expand_name (group);
	expand_name (volume);

	return "/dev/mapper/" + group + "-" + volume;
}


std::vector<Action>
LvmGroup::get_actions (void)
{
	LOG_TRACE;
	std::vector<Action> actions = {
		{ "dummy.lvm_group", true },
	};

	std::vector<Action> parent_actions = Whole::get_actions();

	actions.insert (std::end (actions), std::begin (parent_actions), std::end (parent_actions));

	return actions;
}

bool
LvmGroup::perform_action (Action action)
{
	if (action.name == "dummy.lvm_group") {
		log_debug ("LvmGroup perform: %s", action.name.c_str());
		return true;
	} else {
		return Whole::perform_action (action);
	}
}


int
LvmGroup::lvm_pvs (ContainerPtr& pieces, std::multimap<std::string,std::string>& deps)
{
	LOG_TRACE;

	std::string command;
	std::vector<std::string> output;
	std::map<std::string,StringNum> tags;
	int added = 0;

	/* Output fields from the 'pvs' command:
         * dev_size          - LVM2_DEV_SIZE          - Size of underlying device in current units.
         * lv_name           - LVM2_LV_NAME           - Name.  LVs created for internal use are enclosed in brackets.
         * lv_uuid           - LVM2_LV_UUID           - Unique identifier.
         * pe_start          - LVM2_PE_START          - Offset to the start of data on the underlying device.
         * pvseg_size        - LVM2_PVSEG_SIZE        - Number of extents in segment.
         * pvseg_start       - LVM2_PVSEG_START       - Physical Extent number of start of segment.
         * pv_attr           - LVM2_PV_ATTR           - Various attributes - see man page.
         * pv_count          - LVM2_PV_COUNT          - Number of PVs.
         * pv_free           - LVM2_PV_FREE           - Total amount of unallocated space in current units.
         * pv_name           - LVM2_PV_NAME           - Name.
         * pv_pe_alloc_count - LVM2_PV_PE_ALLOC_COUNT - Total number of allocated Physical Extents.
         * pv_pe_count       - LVM2_PV_PE_COUNT       - Total number of Physical Extents.
         * pv_size           - LVM2_PV_SIZE           - Size of PV in current units.
         * pv_used           - LVM2_PV_USED           - Total amount of allocated space in current units.
         * pv_uuid           - LVM2_PV_UUID           - Unique identifier.
         * segtype           - LVM2_SEGTYPE           - Type of LV segment.
         * vg_extent_size    - LVM2_VG_EXTENT_SIZE    - Size of Physical Extents in current units.
         * vg_name           - LVM2_VG_NAME           - Name.
         * vg_seqno          - LVM2_VG_SEQNO          - Revision number of internal metadata.  Incremented whenever it changes.
         * vg_uuid           - LVM2_VG_UUID           - Unique identifier.
	 */

	command = "sudo pvs --unquoted --separator='\t' --units=b --nosuffix --nameprefixes --noheadings --options "
			"pv_uuid,pv_size,pv_used,pv_attr,vg_extent_size,pvseg_start,pvseg_size,vg_name,vg_uuid,lv_uuid,lv_attr,segtype";

	for (auto i : pieces->get_children()) {
		if (i->is_a ("LvmTable")) {
			log_info ("command: %s", i->get_device_name().c_str());
			command += " " + i->get_device_name();
		}
	}

	execute_command1 (command, output);

	for (auto line : output) {
		log_debug ("OUT: %s", line.c_str());
		parse_tagged_line (line, "\t", tags);

		LvmPartitionPtr p = LvmPartition::create();
		log_debug ("new LvmPartition (%p)", (void*) p.get());

		// Find our relations
		std::string vg_uuid = tags["LVM2_VG_UUID"];
		LvmGroupPtr g = std::dynamic_pointer_cast<LvmGroup> (find_first_uuid (pieces, vg_uuid));
		if (!g) {
			g = LvmGroup::create();
			g->name    = tags["VG_NAME"];
			g->uuid    = vg_uuid;
			//g->missing = true;
			pieces->just_add_child(g);
			++added;
		}

		std::string pv_uuid = tags["LVM2_PV_UUID"];
		LvmTablePtr t = std::dynamic_pointer_cast<LvmTable> (find_first_uuid (pieces, pv_uuid));
		if (!t) {
			log_info ("new table %s [SHOULDN'T HAPPEN]", pv_uuid.c_str());
			t = LvmTable::create();
			t->uuid    = pv_uuid;
			//t->missing = true;
			pieces->just_add_child(t);
			++added;
		}

		ContainerPtr c(t);
		g->add_segment(c);	// Connect the LvmGroup to its constituent LvmTables
		//XXX deps.insert (std::make_pair (g->uuid,t->uuid));

		std::uint64_t size   = tags["LVM2_PVSEG_SIZE"];
		std::uint64_t offset = tags["LVM2_PVSEG_START"];
		size   *= (std::uint64_t) tags["LVM2_VG_EXTENT_SIZE"];
		offset *= (std::uint64_t) tags["LVM2_VG_EXTENT_SIZE"];

		std::string lv_uuid = tags["LVM2_LV_UUID"];
		LvmVolumePtr v = std::dynamic_pointer_cast<LvmVolume> (find_first_uuid (pieces, lv_uuid));
		if (!v) {
			std::string segtype = tags["LVM2_SEGTYPE"];
			std::string lv_attr = tags["LVM2_LV_ATTR"];
			if ((lv_attr[0] == 'e') || (lv_attr[0] == 'l')) {
				log_info ("not a real volume %s", lv_uuid.c_str());
				v = LvmLinear::create();
				v->sub_type ("LvmMetadata");
			} else if (segtype == "linear") {
				v = LvmLinear::create();
			} else if (segtype == "striped") {
				v = LvmStripe::create();
			} else if (segtype == "mirror") {
				v = LvmStripe::create();
			} else if (segtype == "free") {
				PartitionPtr p = Partition::create();
				p->sub_type ("Space");
				p->sub_type ("Unallocated");
				p->bytes_size = size;
				p->bytes_used = size;
				p->parent_offset = offset;
				t->add_child(p);
				continue;
			} else {
				log_error ("UNKNOWN type %s", segtype.c_str());
				continue;
			}
			v->bytes_size = size;

			log_debug ("lv uuid = %s", lv_uuid.c_str());
			v->uuid    = lv_uuid;
			//v->missing = true;
			pieces->just_add_child(v);
			++added;

			if (lv_attr[0] == '-') {
				// Not an image.  Therefore, it's a top-level entity.
				deps.insert (std::make_pair (g->uuid,v->uuid));
			}
		}

		// Container
		p->block_size	= tags["LVM2_VG_EXTENT_SIZE"];
		p->uuid		= t->get_device_name() + "(" + tags["LVM2_PVSEG_START"] + ")";
		p->whole	= nullptr;	//XXX we don't know this yet

		// LvmTable
		t->pv_attr	= tags["LVM2_PV_ATTR"];
		//t->bytes_used	= tags["LVM2_PV_USED"];

		//XXX for now rely on the LvmTable header on disk
		//t->bytes_size	= tags["LVM2_PV_SIZE"];		//XXX this excludes the Reserved and Space

		// LvmPartition
		p->parent_offset  = offset;
		p->bytes_size	  = size;
		p->bytes_used     = size;		//XXX for now

		log_info ("add piece: %s (%s)", p->uuid.c_str(), p->name.c_str());

		t->add_child (p);
	}

	//XXX tag all the lvm_tables as missing
	//XXX tag them as !missing as we add them to the volume group
	//XXX delete all the !missing tables

#if 0
	// remove_if puts all the tables at the end of the vector
	auto end = remove_if (pieces->children.begin(), pieces->children.end(), [] (ContainerPtr c) { return c->is_a ("LvmTable"); });
	// then we delete them
	pieces->children.erase (end, pieces->children.end());
#endif

	return added;
}

void
LvmGroup::lvm_vgs (ContainerPtr& pieces, std::multimap<std::string,std::string>& UNUSED(deps))
{
	return_if_fail (pieces);
	LOG_TRACE;

	std::string command;
	std::vector<std::string> output;
	std::map<std::string,StringNum> tags;

	/* LVM2_VG_NAME=shuffle
	 * LVM2_PV_COUNT=1
	 * LVM2_LV_COUNT=1
	 * LVM2_VG_ATTR=wz--n-
	 * LVM2_VG_SIZE=205520896
	 * LVM2_VG_FREE=0
	 * LVM2_VG_UUID=Usi3h1-mPFH-Z7kS-JNdQ-lron-H8CN-6dyTsC
	 * LVM2_VG_EXTENT_SIZE=4194304
	 * LVM2_VG_EXTENT_COUNT=49
	 * LVM2_VG_FREE_COUNT=0
	 * LVM2_VG_SEQNO=11
	 * LVM2_PV_NAME=/dev/loop3
	 */

	command = "sudo vgs --unquoted --separator='\t' --units=b --nosuffix --nameprefixes --noheadings --options "
		"vg_uuid,vg_name,vg_seqno,pv_count,lv_count,vg_attr,vg_size,vg_free,vg_extent_size";

	for (auto i : pieces->get_children()) {
		if (i->is_a ("LvmGroup"))
			command += " " + i->name;
	}

	execute_command1 (command, output);

	for (auto line : output) {
		log_debug ("OUT: %s", line.c_str());
		parse_tagged_line (line, "\t", tags);

		std::string vg_uuid = tags["LVM2_VG_UUID"];
		LvmGroupPtr g = std::dynamic_pointer_cast<LvmGroup>(find_first_uuid (pieces, vg_uuid));
		if (!g) {
			log_info ("new group %s [SHOULDN'T HAPPEN]", vg_uuid.c_str());
			g = LvmGroup::create();
			g->uuid    = vg_uuid;
			//g->missing = true;
		}

		// Container
		g->name			= tags["LVM2_VG_NAME"];
		g->bytes_size		= tags["LVM2_VG_SIZE"];
		g->block_size		= tags["LVM2_VG_EXTENT_SIZE"];
		//XXX if group has no segments, then what?
		for (auto s : g->segments) {
			LvmTablePtr t = std::dynamic_pointer_cast<LvmTable>(s);
			if (!t) {
				//XXX bad children
			}
			t->set_alignment (g->block_size);
		}
		log_debug ("TRIGGER set_alignment: %ld", g->segments.size());
		//g->bytes_used		= g->bytes_size - (std::uint64_t) tags["LVM2_VG_FREE"];

		// LvmGroup
		g->vg_attr		= tags["LVM2_VG_ATTR"];
		g->pv_count		= tags["LVM2_PV_COUNT"];
		g->lv_count		= tags["LVM2_LV_COUNT"];
		g->vg_seqno		= tags["LVM2_VG_SEQNO"];
	}
}

void
LvmGroup::lvm_lvs (ContainerPtr& pieces, std::multimap<std::string,std::string>& deps)
{
	return_if_fail (pieces);
	LOG_TRACE;

	std::string command;
	std::vector<std::string> output;
	std::map<std::string,StringNum> tags;

	/* LVM2_VG_UUID=Usi3h1-mPFH-Z7kS-JNdQ-lron-H8CN-6dyTsC
	 * LVM2_VG_NAME=shuffle
	 * LVM2_LV_NAME=jigsaw
	 * LVM2_LV_ATTR=-wi-a-
	 * LVM2_MIRROR_LOG=
	 * LVM2_LV_UUID=MjxcHc-tfc6-ZfBV-Zlta-59eS-cpud-dojf09
	 * LVM2_LV_SIZE=205520896
	 * LVM2_LV_PATH=/dev/shuffle/jigsaw
	 * LVM2_LV_KERNEL_MAJOR=253
	 * LVM2_LV_KERNEL_MINOR=2
	 * LVM2_SEG_COUNT=4
	 * LVM2_SEGTYPE=linear
	 * LVM2_STRIPES=1
	 * LVM2_STRIPE_SIZE=0
	 * LVM2_SEG_START_PE=0
	 * LVM2_SEG_PE_RANGES=/dev/loop3:0-12
	 * LVM2_DEVICES=/dev/loop3(0)
	 */

	command = "sudo lvs --all --unquoted --separator='\t' --units=b --nosuffix --noheadings --nameprefixes --sort lv_kernel_minor --options "
		  "vg_uuid,lv_uuid,lv_name,lv_attr,mirror_log,lv_size,lv_path,lv_kernel_major,lv_kernel_minor,seg_count,segtype,stripes,stripe_size,seg_start_pe,devices";

	for (auto i : pieces->get_children()) {
		if (i->is_a ("LvmGroup"))
			command += " " + i->name;
	}

	execute_command1 (command, output);

	for (auto line : output) {
		log_debug ("OUT: %s", line.c_str());
		parse_tagged_line (line, "\t", tags);

		std::string vg_uuid = tags["LVM2_VG_UUID"];
		LvmGroupPtr g = std::dynamic_pointer_cast<LvmGroup> (find_first_uuid (pieces, vg_uuid));
		if (!g) {
			log_info ("new group %s [SHOULDN'T HAPPEN]", vg_uuid.c_str());
			g = LvmGroup::create();
			g->uuid    = vg_uuid;
			//g->missing = true;
			pieces->just_add_child(g);
		}

		std::string lv_uuid = tags["LVM2_LV_UUID"];
		LvmVolumePtr v = std::dynamic_pointer_cast<LvmVolume> (find_first_uuid (pieces, lv_uuid));
		if (!v) {
			std::string segtype = tags["LVM2_SEGTYPE"];
			log_info ("Type = %s", segtype.c_str());
			if (segtype == "linear") {
				v = LvmLinear::create();
			} else if (segtype == "striped") {
				v = LvmStripe::create();
			} else if (segtype == "mirror") {
				v = LvmMirror::create();
			} else if (segtype == "raid1") {
				v = LvmRaid::create();
			} else if (segtype == "raid4") {
				v = LvmRaid::create();
			} else if (segtype == "raid5") {
				v = LvmRaid::create();
			} else if (segtype == "raid6") {
				v = LvmRaid::create();
			} else if (segtype == "raid10") {
				v = LvmRaid::create();
			} else {
				log_error ("UNKNOWN type %s", segtype.c_str());
				continue;
			}
			v->uuid    = lv_uuid;
			//v->missing = true;
			pieces->just_add_child(v);

			// A volume discovered here doesn't have any physical parts.
			// Therefore, it's a top-level entity.
			deps.insert (std::make_pair (g->uuid,v->uuid));
		}

		// Container
		v->name			= tags["LVM2_LV_NAME"];
		v->device		= tags["LVM2_LV_PATH"];		// "/dev/mapper/" + vg_name + "-" + lv_name;
		v->bytes_size		= tags["LVM2_LV_SIZE"];

		// LvmVolume
		v->lv_attr		= tags["LVM2_LV_ATTR"];		// m(e)tadata, (i)mage, (r)aid
		v->device_major		= tags["LVM2_LV_KERNEL_MAJOR"];
		v->device_minor		= tags["LVM2_LV_KERNEL_MINOR"];

		v->seg_count		= tags["LVM2_SEG_COUNT"];
		v->stripes		= tags["LVM2_STRIPES"];
		v->stripe_size		= tags["LVM2_STRIPE_SIZE"];
		v->seg_start_pe		= tags["LVM2_SEG_START_PE"];
		v->mirror_log		= tags["LVM2_MIRROR_LOG"];

		log_info ("name = %s, attr = %s", v->name.c_str(), v->lv_attr.c_str());
		if ((v->lv_attr[0] == 'i') ||
		    (v->lv_attr[0] == 'I') ||
		    (v->lv_attr[0] == 'l') ||
		    (v->lv_attr[0] == 'e')) {		// mirror image/log, raid metadata/image
			v->name = v->name.substr (1, v->name.length() - 2);	// Strip the []'s

			v->device = make_device (g->name, v->name);
			log_debug ("DEVNAME = %s", v->device.c_str());

			//INTERNAL object => full
			v->bytes_used = v->bytes_size;
		}

		std::string devices = tags["LVM2_DEVICES"];
		std::vector<std::string> device_list;
		explode (",", devices, device_list);

		log_debug ("%s (%s)", v->name.c_str(), v->type.back().c_str());
		for (auto d : device_list) {
			log_info ("Device: %s", d.c_str());
		}

		for (auto d : device_list) {
			log_info ("DEP %s -> %s", v->uuid.c_str(), d.c_str());
			deps.insert (std::make_pair (v->uuid,d));

			log_info ("attr = %s", v->lv_attr.c_str());
			// Add a metadata dependency
			if (v->lv_attr[0] == 'r') {
				log_info ("raid metadata");
				// Add an extra dependency for raid metadata
				std::string dep_name = d;
				std::size_t pos = dep_name.find ("_rimage_");
				if (pos != std::string::npos) {
					dep_name.replace (pos, 8, "_rmeta_");
					log_debug ("ADD: %s -> %s", v->uuid.c_str(), dep_name.c_str());
					deps.insert (std::make_pair (v->uuid,dep_name));

					// add a sibling dependency (image <-> metadata)
					log_info ("SIBLING: %s <-> %s", dep_name.c_str(), d.c_str());
					deps.insert (std::make_pair (dep_name, d));
				}
			}
		}

		if (v->lv_attr[0] == 'm') {
			// Add an extra dependency for mirrors
			std::string dep_name = v->mirror_log + "(0)";
			log_debug ("ADD DEP: %s -> %s", v->uuid.c_str(), dep_name.c_str());
			deps.insert (std::make_pair (v->uuid,dep_name));
		}
	}

	//for (int j = 0; j < 5; ++j) {
	for (auto d : deps) {
		std::string parent_id = d.first;
		std::string child_id  = d.second;

		log_info ("DEPS: %s -> %s", parent_id.c_str(), child_id.c_str());

		ContainerPtr parent = pieces->find (parent_id);
		if (!parent) {
			log_error ("\tcan't find parent: %s", parent_id.c_str());
			continue;
		}

		ContainerPtr child  = pieces->find (child_id);
		if (!child) {
			log_error ("\tcan't find child: %s", child_id.c_str());
			continue;
		}

		log_info ("add_child %s -> %s", parent->uuid.c_str(), child->uuid.c_str());
		parent->add_child (child);

		log_info ("\t%p -> %p", (void*) parent.get(), (void*) child.get());
		log_info ("\t%s -> %s", parent->name.c_str(), child->name.c_str());
		log_info ("\t%s -> %s", parent->uuid.c_str(), child->uuid.c_str());

		pieces->delete_child (child);
	}
	deps.clear();
}


void
LvmGroup::discover (ContainerPtr& top_level)
{
	return_if_fail (top_level);
	LOG_TRACE;

	ContainerPtr pieces = Container::create();
	std::multimap<std::string,std::string> deps;

	std::vector<ContainerPtr> t = find_all_type (top_level, "LvmTable");

	log_info ("top_level: %ld tables", t.size());
	for (auto i : t) {
		pieces->just_add_child(i);
	}

	if (lvm_pvs (pieces, deps) > 0) {
		lvm_vgs (pieces, deps);
		lvm_lvs (pieces, deps);
	}

	log_info ("Pieces (%ld)", pieces->get_children().size());
	for (auto i : pieces->get_children()) {
		log_debug ("\t%s\t%s", i->uuid.c_str(), i->dump());
	}

	//probe leaves
	std::vector<ContainerPtr> v = find_all_type (pieces, "LvmVolume");
	log_debug ("%ld volumes", v.size());

	for (auto i : v) {
		if (i->is_a ("LvmMetadata"))		// nothing interesting here
			continue;

		if (i->whole)				// we're part of something bigger
			continue;

		log_info ("Q: [%s] %s: %s", i->type.back().c_str(), i->name.c_str(), i->uuid.c_str());
		main_app->queue_add_probe(i);
	}

	std::vector<ContainerPtr> g = find_all_type (pieces, "LvmGroup");
	log_debug ("%ld groups", g.size());

	for (auto i : g) {
		log_debug ("\t%s\t%s", i->uuid.c_str(), i->dump());
		top_level->just_add_child(i);
	}
}


