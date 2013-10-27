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

#include <iterator>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <algorithm>

#include "log.h"
#include "lvm_group.h"
#include "lvm_linear.h"
#include "lvm_mirror.h"
#include "lvm_partition.h"
#include "lvm_stripe.h"
#include "lvm_table.h"
#include "lvm_volume.h"
#include "main.h"
#include "utils.h"
#include "log_trace.h"

/**
 * LvmGroup
 */
LvmGroup::LvmGroup (void) :
	pv_count (0),
	lv_count (0),
	vg_seqno (0)
{
	declare ("lvm_group");
}

/**
 * ~LvmGroup
 */
LvmGroup::~LvmGroup()
{
}


/**
 * fd_pvs
 */
void
fd_pvs (std::map<std::string, LvmTable*>     &tables,
	 std::map<std::string, LvmPartition*> &partitions,
	 std::map<std::string, LvmGroup*>     &groups,
	 std::map<std::string, LvmVolume*>    &volumes)
{
	//LOG_TRACE;

	std::string command;
	std::vector<std::string> output;
	std::string error;
	std::map<std::string,StringNum> tags;

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
			"pv_uuid,pv_size,pv_used,pv_attr,vg_extent_size,pvseg_start,pvseg_size,vg_name,vg_uuid,lv_uuid,segtype";

	for (auto i : tables) {
		command += " " + i.second->get_device_name();
	}

	execute_command (command, output);

	for (auto line : output) {
		//log_debug ("OUT: %s\n", line.c_str());
		parse_tagged_line (line, "\t", tags);

		std::string segtype = tags["LVM2_SEGTYPE"];
		if (segtype == "free")		//XXX could process this and add it to the VGSeg's empty list
			continue;

		LvmPartition *p = new LvmPartition;
		//log_debug ("new LvmPartition (%p)\n", p);

		// Find our relations
		std::string vg_uuid = tags["LVM2_VG_UUID"];
		LvmGroup *g = groups[vg_uuid];
		if (!g) {
			g = new LvmGroup();
			g->name    = tags["VG_NAME"];
			g->uuid    = vg_uuid;
			g->missing = true;
			groups[vg_uuid] = g;
		}

		std::string pv_uuid = tags["LVM2_PV_UUID"];
		LvmTable *t = tables[pv_uuid];
		if (!t) {
			printf ("new table %s [SHOULDN'T HAPPEN]\n", pv_uuid.c_str());
			t = new LvmTable();
			t->uuid    = pv_uuid;
			t->missing = true;
			tables[pv_uuid] = t;
		}

		g->add_segment(t);	// Connect the LvmGroup to its constituent LvmTables

		std::string lv_uuid = tags["LVM2_LV_UUID"];
		LvmVolume *v = volumes[lv_uuid];
		if (!v) {
			if (segtype == "linear") {
				v = new LvmLinear();
			} else if (segtype == "striped") {
				v = new LvmStripe();
			} else if (segtype == "mirror") {
				v = new LvmMirror();
			} else {
				log_error ("UNKNOWN type %s\n", segtype.c_str());
				continue;
			}

			v->uuid    = lv_uuid;
			v->missing = true;
			volumes[lv_uuid] = v;
		}

		// DPContainer
		p->name		= "partition";
		p->block_size	= tags["LVM2_VG_EXTENT_SIZE"];
		p->uuid		= t->get_device_name() + "(" + tags["LVM2_PVSEG_START"] + ")";
		p->whole	= g;

		// LvmTable
		t->attr		= tags["LVM2_PV_ATTR"];
		t->bytes_size	= tags["LVM2_PV_SIZE"];
		//t->bytes_used	= tags["LVM2_PV_USED"];

		// LvmPartition
		p->parent_offset  = tags["LVM2_PVSEG_START"];
		p->parent_offset *= p->block_size;
		p->bytes_size	  = tags["LVM2_PVSEG_SIZE"];
		p->bytes_size	 *= p->block_size;
		p->bytes_used     = p->bytes_size;	//XXX for now

		partitions[p->uuid] = p;

		t->add_child (p);
	}
}

/**
 * fd_vgs
 */
void
fd_vgs (std::map<std::string, LvmGroup*> &groups)
{
	//LOG_TRACE;

	std::string command;
	std::vector<std::string> output;
	std::string error;
	std::map<std::string,StringNum> tags;

	/*
	 * LVM2_VG_NAME=shuffle
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

	for (auto i : groups) {
		command += " " + i.second->name;
	}

	execute_command (command, output);

	for (auto line : output) {
		//log_debug ("OUT: %s\n", line.c_str());
		parse_tagged_line (line, "\t", tags);

		std::string vg_uuid = tags["LVM2_VG_UUID"];
		LvmGroup *g = groups[vg_uuid];
		if (!g) {
			printf ("new group %s [SHOULDN'T HAPPEN]\n", vg_uuid.c_str());
			g = new LvmGroup();
			g->uuid    = vg_uuid;
			g->missing = true;
			groups[vg_uuid] = g;
		}

		// DPContainer
		g->name			= tags["LVM2_VG_NAME"];
		g->block_size		= tags["LVM2_VG_EXTENT_SIZE"];
		g->bytes_size		= tags["LVM2_VG_SIZE"];
		g->bytes_used		= g->bytes_size - (long) tags["LVM2_VG_FREE"];

		// LvmGroup
		g->vg_attr		= tags["LVM2_VG_ATTR"];
		g->pv_count		= tags["LVM2_PV_COUNT"];
		g->lv_count		= tags["LVM2_LV_COUNT"];
		g->vg_seqno		= tags["LVM2_VG_SEQNO"];
	}
}

/**
 * fd_lvs
 */
void
fd_lvs (std::map<std::string, LvmGroup*>  &groups,
	 std::map<std::string, LvmVolume*> &volumes)
{
	//LOG_TRACE;

	std::string command;
	std::vector<std::string> output;
	std::string error;
	std::map<std::string,StringNum> tags;

	/*
	 * LVM2_VG_UUID=Usi3h1-mPFH-Z7kS-JNdQ-lron-H8CN-6dyTsC
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

	for (auto i : groups) {
		command += " " + i.second->name;
	}

	execute_command (command, output);

	for (auto line : output) {
		//log_debug ("OUT: %s\n", line.c_str());
		parse_tagged_line (line, "\t", tags);

		std::string vg_uuid = tags["LVM2_VG_UUID"];
		LvmGroup *g = groups[vg_uuid];
		if (!g) {
			printf ("new group %s [SHOULDN'T HAPPEN]\n", vg_uuid.c_str());
			g = new LvmGroup();
			g->uuid    = vg_uuid;
			g->missing = true;
			groups[vg_uuid] = g;
		}

		std::string lv_uuid = tags["LVM2_LV_UUID"];
		LvmVolume *v = volumes[lv_uuid];
		if (!v) {
			std::string segtype = tags["LVM2_SEGTYPE"];
			if (segtype == "linear") {
				v = new LvmLinear();
			} else if (segtype == "striped") {
				v = new LvmStripe();
			} else if (segtype == "mirror") {
				v = new LvmMirror();
			} else {
				log_error ("UNKNOWN type %s\n", segtype.c_str());
				continue;
			}
			v->uuid    = vg_uuid;
			v->missing = true;
			volumes[lv_uuid] = v;
		}

		// DPContainer
		v->name			= tags["LVM2_LV_NAME"];
		v->device		= tags["LVM2_LV_PATH"];		// "/dev/mapper/" + vg_name + "-" + lv_name;
		v->bytes_size		= tags["LVM2_LV_SIZE"];

		// LvmVolume
		v->lv_attr		= tags["LVM2_LV_ATTR"];
		v->kernel_major		= tags["LVM2_LV_KERNEL_MAJOR"];
		v->kernel_minor		= tags["LVM2_LV_KERNEL_MINOR"];

		v->seg_count		= tags["LVM2_SEG_COUNT"];
		v->stripes		= tags["LVM2_STRIPES"];
		v->stripesize		= tags["LVM2_STRIPESIZE"];
		v->stripe_size		= tags["LVM2_STRIPE_SIZE"];
		v->seg_start_pe		= tags["LVM2_SEG_START_PE"];
		v->mirror_log		= tags["LVM2_MIRROR_LOG"];

		if ((v->lv_attr[0] == 'i') || (v->lv_attr[0] == 'I') || (v->lv_attr[0] == 'l')) {		// mirror image/log
			v->name = v->name.substr (1, v->name.length() - 2);	// Strip the []'s
		}

		std::string devices = tags["LVM2_DEVICES"];
		std::vector<std::string> device_list;
		explode (",", devices, device_list);

		log_debug ("%s (%s)\n", v->name.c_str(), v->type.back().c_str());

		if (v->lv_attr[0] == 'm') {
			// Add an extra dependency for mirrors
			std::string dep_name = v->mirror_log + "(0)";
			//log_debug ("ADD DEP: %s\n", dep_name.c_str());
			device_list.push_back (dep_name);
		}

		std::string vol_id = g->name + ":" + v->name;
		//log_info ("\t%s\n", vol_id.c_str());

		for (auto dep : device_list) {
			log_debug ("\t%s\n", dep.c_str());

#if 0
			std::map<std::string, DPContainer*>::iterator it_vol_seg;
			it_vol_seg = vol_seg_lookup.find (dep);
			if (it_vol_seg != vol_seg_lookup.end()) {
				//log_debug ("found it (%s)\n", lv_name.c_str());
			} else {
				//log_debug ("vol_seg_lookup: looking for %s: ", dep.c_str());
				//log_debug ("didn't find it (%s)\n", lv_name.c_str());
			}
			if (it_vol_seg != vol_seg_lookup.end()) {
				//RAR connect up stuff
				DPContainer *s_dep = (*it_vol_seg).second;
				//log_debug ("vol_seg_lookup: %s -> %s\n", dep.c_str(), v->name.c_str());
				v->add_segment (s_dep);
				//log_debug ("add_segment %p [%s/%s] -> %p [%s/%s]\n", v, v->type.back().c_str(), v->name.c_str(), s_dep, s_dep->type.back().c_str(), s_dep->name.c_str());
				vol_seg_lookup.erase (it_vol_seg);
				//log_debug ("vol_seg_lookup: erasing %s\n", (*it_vol_seg).first.c_str());
				continue;
			}

			dep = vg_name + ":" + dep;
			size_t pos = dep.find ('(');
			if (pos != std::string::npos) {
				dep = dep.substr (0, pos);
			}
			std::map<std::string, LvmVolume*>::iterator it_vol;
			it_vol = vol_lookup.find (dep);
			if (it_vol != vol_lookup.end()) {
				LvmVolume *v_dep = (*it_vol).second;
				//RAR connect up stuff
				v->add_child (v_dep);
				//log_debug ("add_child %p [%s/%s]-> %p [%s/%s]\n", v, v->type.back().c_str(), v->name.c_str(), v_dep, v_dep->type.back().c_str(), v_dep->name.c_str());
				vol_lookup.erase (it_vol);
				continue;
			}

			//log_error ("DEP: couldn't find %s\n", dep.c_str());
#endif
		}
	}

#if 0
	//transfer volumes to disks
	for (auto it_vol : vol_lookup) {
		LvmVolume *v = it_vol.second;
		//log_debug ("volume %s (%s)\n", v->name.c_str(), v->parent->name.c_str());
		v->parent->add_child (v);	//RAR add myself to MY parent
		//log_debug ("add_child %p [%s/%s] -> %p [%s/%s]\n", v->parent, v->parent->type.back().c_str(), v->parent->name.c_str(), v, v->type.back().c_str(), v->name.c_str());
		fd_probe_children (v);
	}
#endif
}


/**
 * discover
 */
void
LvmGroup::discover (DPContainer &top_level)
{
	//LOG_TRACE;

	std::map<std::string, LvmTable*>     tables;
	std::map<std::string, LvmPartition*> partitions;
	std::map<std::string, LvmGroup*>     groups;
	std::map<std::string, LvmVolume*>    volumes;

	std::vector<DPContainer*> t;
	top_level.find_type ("lvm_table", t);

	for (auto i : t) {
		tables[i->uuid] = dynamic_cast<LvmTable*>(i);
	}

	fd_pvs (tables, partitions, groups, volumes);
	fd_vgs (groups);
	fd_lvs (groups, volumes);

#if 1
	printf ("tables (%ld)\n",     tables.size());     for (auto i : tables)     std::cout << '\t' << i.second << '\n';
	printf ("partitions (%ld)\n", partitions.size()); for (auto i : partitions) std::cout << '\t' << i.second << '\n';
	printf ("groups (%ld)\n",     groups.size());     for (auto i : groups)     std::cout << '\t' << i.second << '\n';
	printf ("volumes (%ld)\n",    volumes.size());    for (auto i : volumes)    std::cout << '\t' << i.second << '\n';
#endif

	for (auto i : groups)  top_level.add_child (i.second);
	for (auto i : volumes) top_level.add_child (i.second);
#if 0
	//probe leaves
	std::vector<DPContainer*> v;
	top_level.find_type ("lvm_volume", v);

	for (auto i : v) {
		queue_add_probe (i);
	}
#endif
}

