/* Copyright (c) 2012 Richard Russon (FlatCap)
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

//RAR lazy

std::map<std::string, LVMGroup*>     vg_lookup;		// UUID            -> LVMGroup		(purple)
							// I9a0Vj...       -> tmpvol
std::map<std::string, LVMVolume*>    vol_lookup;	// VG_NAME:LV_NAME -> LVMVolume		(purple)
							// tmpvol:stripe2  -> stripe2
std::map<std::string, LVMTable*>     vg_seg_lookup;	// device          -> LVMTable		(pink)
							// /dev/loop0      -> tmpvol
std::map<std::string, DPContainer*>  vol_seg_lookup;	// device(offset)  -> LVMPartition	(yellow)
							// /dev/loop0(0)   -> stripe2

/**
 * LVMGroup
 */
LVMGroup::LVMGroup (void) :
	pv_count (0),
	lv_count (0),
	vg_extent_count (0),
	vg_free_count (0),
	vg_seqno (0)
{
	declare ("lvm_group");
}

/**
 * ~LVMGroup
 */
LVMGroup::~LVMGroup()
{
}


/**
 * dump_map
 */
template<typename T>
void dump_map (const char *title, const T &m)
{
	log_debug ("%s:\n", title);
	for (auto i : m) {
		DPContainer *c = dynamic_cast<DPContainer*>(i.second);

		log_debug ("\t%s => %s [%s] (%p)\n", i.first.c_str(), c->name.c_str(), c->type.back().c_str(), c);
	}
}


/**
 * fd_probe_children
 */
void fd_probe_children (DPContainer *item)
{
	// recurse through children and add_probe()
	if (item->children.size() == 0) {
		//log_info ("PROBE: %s\n", item->name.c_str());
		if (!item->is_a ("volume"))	//RAR tmp
			queue_add_probe (item);
	}

	for (auto child : item->children) {
		fd_probe_children (child);
	}
}

/**
 * fd_vgs - Create the LVMGroup objects
 */
void fd_vgs (DPContainer &disks)
{
	std::string command;
	std::vector<std::string> output;
	std::string error;
	std::map<std::string,StringNum> tags;
	LVMGroup *vg = NULL;

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

	command = "sudo vgs --unquoted --separator='\t' --units=b --nosuffix --nameprefixes --noheadings --options vg_name,pv_count,lv_count,vg_attr,vg_size,vg_free,vg_uuid,vg_extent_size,vg_extent_count,vg_free_count,vg_seqno,pv_name,pv_uuid";
	execute_command (command, output);

	for (auto line : output) {
		parse_tagged_line (line, "\t", tags);

		std::string vg_uuid = tags["LVM2_VG_UUID"];
		std::string pv_name = tags["LVM2_PV_NAME"];
		std::string pv_uuid = tags["LVM2_PV_UUID"];

		vg = vg_lookup[vg_uuid];
		if (vg == NULL) {
			vg = new LVMGroup;
			//log_debug ("new LVMGroup (%p)\n", vg);
			vg->parent = &disks;	//RAR not nec (done in add_child)
			//vg->device = "/dev/dm-0"; //RAR what?
			// LVMGroup doesn't have a dedicated device, so it would have to delegate to the vg segment

			vg->name		= tags["LVM2_VG_NAME"];
			vg->pv_count		= tags["LVM2_PV_COUNT"];
			vg->lv_count		= tags["LVM2_LV_COUNT"];
			vg->vg_attr		= tags["LVM2_VG_ATTR"];
			vg->bytes_size		= tags["LVM2_VG_SIZE"];
			//vg->vg_free		= tags["LVM2_VG_FREE"];
			vg->uuid		= vg_uuid;
			vg->block_size		= tags["LVM2_VG_EXTENT_SIZE"];
			vg->vg_extent_count	= tags["LVM2_VG_EXTENT_COUNT"];
			vg->vg_free_count	= tags["LVM2_VG_FREE_COUNT"];
			vg->vg_seqno		= tags["LVM2_VG_SEQNO"];

			disks.add_child (vg);

			vg_lookup[vg->uuid] = vg;
			//log_debug ("vg_lookup: %s -> %s\n", vg->uuid.c_str(), vg->name.c_str());
		}

		//std::string seg_id = vg->name + ":" + pv_name;
		//log_debug ("\t%s\n", seg_id.c_str());

		DPContainer *cont = disks.find_device (pv_name);
		//log_debug ("cont for %s = %p\n", pv_name.c_str(), cont);

		LVMTable *vg_seg = NULL;
		//log_info ("find: %s\n", pv_uuid.c_str());
		vg_seg = dynamic_cast<LVMTable*> (cont->find_uuid (pv_uuid));
		if (vg_seg) {
			//log_info ("table already exists: %s\n", pv_uuid.c_str());
		} else {
			log_error ("table didn't already exist: %s\n", pv_uuid.c_str());
			vg_seg = new LVMTable;
			//log_debug ("new LVMTable (%p)\n", vg_seg);
			cont->add_child (vg_seg);	//RAR this could be left to find_devices, there don't need disks fn param
			//log_debug ("add_child %p [%s/%s] -> %p [%s/%s]\n", cont, cont->type.back().c_str(), cont->name.c_str(), vg_seg, vg_seg->type.back().c_str(), vg_seg->name.c_str());
		}

		vg_seg->bytes_size = cont->bytes_size;
		vg_seg->block_size = vg->block_size;
		vg_seg->uuid = pv_uuid;
		vg_seg->name = vg->name;

		//log_debug ("vg block size = %ld\n", vg->block_size);

		//log_debug ("lookup uuid %s\n", vg_uuid.c_str());
		//log_debug ("vg_seg_lookup: %s -> %s\n", pv_name.c_str(), vg->name.c_str());
		vg->add_segment (vg_seg);
		//log_debug ("add_segment %p [%s/%s] -> %p [%s/%s]\n", vg, vg->type.back().c_str(), vg->name.c_str(), vg_seg, vg_seg->type.back().c_str(), vg_seg->name.c_str());

		//log_debug ("whole = %p\n", vg_seg->whole);

		//log_debug ("pv_name = %s\n", pv_name.c_str());
		vg_seg_lookup[pv_name] = vg_seg;
#if 0
		Partition *reserved1 = new Partition;
		//XXX type = metadata/reserved
		// get size from LVM2_PE_START
		reserved1->bytes_size = 1048576;
		reserved1->bytes_used = 1048576;
		reserved1->parent_offset = 0;
		reserved1->type = "metadata";
		vg_seg->add_child (reserved1);
#endif
		//RAR need to calculate this from vg_seg size and vg block size
#if 0
		Partition *reserved2 = new Partition;
		//XXX type = metadata/reserved
		// get size from LVM2_PV_PE_ALLOC_COUNT and LVM2_PE_START
		reserved2->bytes_size = 3145728;
		reserved2->bytes_used = 3145728;
		reserved2->parent_offset = cont->bytes_size - reserved2->bytes_size;
		reserved2->type = "reserved";
		vg_seg->add_child (reserved2);
#endif
	}
	//log_debug ("\n");

	//dump_map ("Volume Group map (vg_lookup)", vg_lookup);
	//dump_map ("Volume Group Segments map (vg_seg_lookup)", vg_seg_lookup);
}

/**
 * fd_pvs - Attach all the Segments (LVMGroup and Volumes)
 */
void fd_pvs (DPContainer &disks)	//RAR disks unused
{
	std::string command;
	std::vector<std::string> output;
	std::string error;
	std::map<std::string,StringNum> tags;

	/*
	 * LVM2_PV_NAME=/dev/loop3
	 * LVM2_LV_NAME=jigsaw
	 * LVM2_LV_UUID=uaAoyD-iscK-5mTi-QJBY-4dwT-Yc1q-kEdi5O
	 * LVM2_LV_ATTR=-wi-a-
	 * LVM2_VG_UUID=Usi3h1-mPFH-Z7kS-JNdQ-lron-H8CN-6dyTsC
	 * LVM2_VG_NAME=shuffle
	 * LVM2_SEGTYPE=linear
	 * LVM2_PVSEG_START=0
	 * LVM2_PVSEG_SIZE=13
	 */

	command = "sudo pvs --unquoted --separator='\t' --units=b --nosuffix --nameprefixes --noheadings --options pv_name,pv_uuid,lv_name,lv_uuid,lv_attr,vg_uuid,vg_name,segtype,pvseg_start,pvseg_size";
	execute_command (command, output);

	for (auto line : output) {
		//log_debug ("OUT: %s\n", line.c_str());
		parse_tagged_line (line, "\t", tags);

		std::string lv_type = tags["LVM2_SEGTYPE"];
		if (lv_type == "free")				//XXX could process this and add it to the VGSeg's empty list
			continue;

		std::string dev     = tags["LVM2_PV_NAME"];	// /dev/loop0
		std::string vg_uuid = tags["LVM2_VG_UUID"];
		std::string vg_name = tags["LVM2_VG_NAME"];
		std::string lv_name = tags["LVM2_LV_NAME"];
		std::string lv_uuid = tags["LVM2_LV_UUID"];
		std::string lv_attr = tags["LVM2_LV_ATTR"];
		std::string pv_name = tags["LVM2_PV_NAME"];
		std::string pv_uuid = tags["LVM2_PV_UUID"];
		std::string start   = tags["LVM2_PVSEG_START"];	// physical volume offset, i.e. how far into /dev/loop0?

		//log_debug ("pv_uuid = %s\n", pv_uuid.c_str());

		if ((lv_attr[0] == 'i') || (lv_attr[0] == 'l')) {		// mirror image/log
			lv_name = lv_name.substr (1, lv_name.length() - 2);	// Strip the []'s
		}

		//std::string seg_id = vg_name + ":" + lv_name + ":" + pv_name + "(" + start + ")";
		std::string seg_id = pv_name + "(" + start + ")";
		//log_debug ("\tseg_id = %s\n", seg_id.c_str());

		//log_debug ("lookup uuid %s\n", vg_uuid.c_str());
		LVMGroup *vg = vg_lookup[vg_uuid];
		//log_debug ("vg extent size = %ld\n", vg->block_size);

		LVMTable *vg_seg = vg_seg_lookup[dev];	//XXX this should exist
		if (!vg_seg)
			continue;			//XXX error?

		LVMPartition *vol_seg = new LVMPartition;
		//log_debug ("new LVMPartition (%p)\n", vol_seg);
		vol_seg->name = lv_name;
		vol_seg->uuid = lv_uuid;
		//vol_seg->type = lv_type;			// linear, striped, etc

		vol_seg->bytes_size = tags["LVM2_PVSEG_SIZE"];
		vol_seg->bytes_size *= vg->block_size;
		vol_seg->parent_offset = tags["LVM2_PVSEG_START"];
		vol_seg->parent_offset *= vg->block_size;

		vol_seg->whole = NULL; //RAR we don't know this yet

		//log_debug ("whole = %p\n", vg_seg->whole);

		vg_seg->add_child (vol_seg);
		//log_debug ("add_child %p [%s/%s] -> %p [%s/%s]\n", vg_seg, vg_seg->type.back().c_str(), vg_seg->name.c_str(), vol_seg, vol_seg->type.back().c_str(), vol_seg->name.c_str());
		//log_debug ("vol_seg_lookup: %s -> %s\n", seg_id.c_str(), vol_seg->name.c_str());
		vol_seg_lookup[seg_id] = vol_seg;

		//std::string vol_id = pv_name + "(" + start + ")";	//XXX dupe
	}
	//log_debug ("\n");

	//dump_map ("Volume Segments map (vol_seg_lookup)", vol_seg_lookup);
}

/**
 * fd_lvs - Build the Volumes from the Segments
 */
void fd_lvs (DPContainer &disks)
{
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

	command = "sudo lvs --all --unquoted --separator='\t' --units=b --nosuffix --noheadings --nameprefixes --sort lv_kernel_minor --options vg_uuid,vg_name,lv_name,lv_attr,mirror_log,lv_uuid,lv_size,lv_path,lv_kernel_major,lv_kernel_minor,seg_count,segtype,stripes,stripe_size,seg_start_pe,seg_pe_ranges,devices";
	execute_command (command, output);

	//log_debug ("Volumes:\n");
	for (auto line : output) {
		//log_debug ("OUT: %s\n", line.c_str());
		parse_tagged_line (line, "\t", tags);

		std::string vg_uuid = tags["LVM2_VG_UUID"];	//log_debug ("vg_uuid = %s\n", vg_uuid.c_str());
		std::string vg_name = tags["LVM2_VG_NAME"];	//log_debug ("vg_name = %s\n", vg_name.c_str());
		std::string lv_name = tags["LVM2_LV_NAME"];	//log_debug ("lv_name = %s\n", lv_name.c_str());
		std::string lv_attr = tags["LVM2_LV_ATTR"];	//log_debug ("lv_attr = %s\n", lv_attr.c_str());
		std::string lv_uuid = tags["LVM2_LV_UUID"];	//log_debug ("lv_uuid = %s\n", lv_uuid.c_str());
		std::string devices = tags["LVM2_DEVICES"];	//log_debug ("devices = %s\n", devices.c_str());
		std::string lv_type = tags["LVM2_SEGTYPE"];
		std::string mirrlog = tags["LVM2_MIRROR_LOG"];
		//log_debug ("\n");

		std::vector<std::string> device_list;
		explode (",", devices, device_list);

		if ((lv_attr[0] == 'i') || (lv_attr[0] == 'l')) {		// mirror image/log
			lv_name = lv_name.substr (1, lv_name.length() - 2);	// RAR dupe Strip the []'s
		} else if (lv_attr[0] == 'm') {
			// Add an extra dependency for mirrors
			std::string dep_name = mirrlog + "(0)";
			//log_debug ("ADD DEP: %s\n", dep_name.c_str());
			device_list.push_back (dep_name);
		}

		std::string vol_id = vg_name + ":" + lv_name;
		//log_info ("\t%s\n", vol_id.c_str());

		LVMVolume *v = vol_lookup[vol_id];
		if (v == NULL) {
			if (lv_type == "linear") {
				v = new LVMLinear;
				//log_debug ("new LVMLinear (%p)\n", v);
				std::string seg_id = lv_name + "(0)"; // parent offset
				vol_seg_lookup[seg_id] = v;
			} else if (lv_type == "striped") {
				v = new LVMStripe;
				//log_debug ("new LVMStripe (%p)\n", v);
				std::string seg_id = lv_name + "(0)"; // parent offset
				vol_seg_lookup[seg_id] = v;
			} else if (lv_type == "mirror") {
				v = new LVMMirror;
				//log_debug ("new LVMMirror (%p)\n", v);
				std::string seg_id = lv_name + "(0)"; // parent offset
				vol_seg_lookup[seg_id] = v;
			} else {
				log_error ("UNKNOWN type %s\n", lv_type.c_str());
				continue;
			}

			v->name = lv_name;			//RAR and other details...
			v->uuid = lv_uuid;
			v->lv_attr = lv_attr;
			v->bytes_size = tags["LVM2_LV_SIZE"];
			v->device = "/dev/mapper/" + vg_name + "-" + lv_name;
			v->parent_offset = 0;
			v->parent = vg_lookup[vg_uuid];		//XXX until we know better

			vol_lookup[vol_id] = v;
		}

		for (auto dep : device_list) {
			//log_debug ("\t\t%s\n", dep.c_str());

			std::map<std::string, DPContainer*>::iterator it_vol_seg;
			it_vol_seg = vol_seg_lookup.find (dep);
			if (it_vol_seg != vol_seg_lookup.end()) {
				//log_debug ("found it (%s)\n", lv_name.c_str());
			} else {
				log_debug ("vol_seg_lookup: looking for %s: ", dep.c_str());
				log_debug ("didn't find it (%s)\n", lv_name.c_str());
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
			std::map<std::string, LVMVolume*>::iterator it_vol;
			it_vol = vol_lookup.find (dep);
			if (it_vol != vol_lookup.end()) {
				LVMVolume *v_dep = (*it_vol).second;
				//RAR connect up stuff
				v->add_child (v_dep);
				//log_debug ("add_child %p [%s/%s]-> %p [%s/%s]\n", v, v->type.back().c_str(), v->name.c_str(), v_dep, v_dep->type.back().c_str(), v_dep->name.c_str());
				vol_lookup.erase (it_vol);
				continue;
			}

			log_error ("DEP: couldn't find %s\n", dep.c_str());
		}
	}

	//dump_map ("Volume Segments map (vol_seg_lookup)", vol_seg_lookup);
	//dump_map ("Volumes map (vol_lookup)", vol_lookup);

	//transfer volumes to disks
	for (auto it_vol : vol_lookup) {
		LVMVolume *v = it_vol.second;
		//log_debug ("volume %s (%s)\n", v->name.c_str(), v->parent->name.c_str());
		v->parent->add_child (v);	//RAR add myself to MY parent
		//log_debug ("add_child %p [%s/%s] -> %p [%s/%s]\n", v->parent, v->parent->type.back().c_str(), v->parent->name.c_str(), v, v->type.back().c_str(), v->name.c_str());
		fd_probe_children (v);
	}
}


/**
 * fd_fs
 */
void fd_fs (void)
{
	// find the all the volumes
	// identify the filesystems
	// create filesystem objects under the matching partition

	for (auto it_vol : vol_lookup) {
		//log_error ("marker\n");
		LVMVolume *v = it_vol.second;

		DPContainer *item = probe (v);
		if (item) {
			//std::cout << item << std::endl;
		} else {
			log_debug ("no match for %s\n", v->device.c_str());
			return;
		}

		//log_debug ("volume %s (%s)\n", v->name.c_str(), v->parent->name.c_str());

		v->add_child (item);
		//log_debug ("add_child %p [%s/%s] -> %p [%s/%s]\n", v, v->type.back().c_str(), v->name.c_str(), item, item->type.back().c_str(), item->name.c_str());

		/*
		for (auto it_seg : v->segments) {
			LVMPartition *p = dynamic_cast<LVMPartition*>(it_seg);
			log_debug ("\t%s (%s) - %p\n", p->name.c_str(), p->parent->name.c_str(), p);
			p->add_child (item);
		}
		*/
	}
}

/**
 * find_devices
 */
void LVMGroup::find_devices (DPContainer &disks)
{
	//log_debug ("%s\nvgs\n", "_________________________________________________________________________________________________________________________\n");
	fd_vgs (disks);
	//log_debug ("%s\npvs\n", "_________________________________________________________________________________________________________________________\n");
	fd_pvs (disks);
	//log_debug ("%s\nlvs\n", "_________________________________________________________________________________________________________________________\n");
	fd_lvs (disks);
	//log_debug ("%s\ndump\n", "_________________________________________________________________________________________________________________________\n");

#if 0
	dump_map ("vg_lookup",      vg_lookup);
	dump_map ("vol_lookup",     vol_lookup);
	dump_map ("vg_seg_lookup",  vg_seg_lookup);
	dump_map ("vol_seg_lookup", vol_seg_lookup);
#endif

	fd_fs();
}


/**
 * dump_dot
 */
std::string LVMGroup::dump_dot (void)
{
	std::ostringstream output;

	output << Whole::dump_dot();

	output << dump_row ("pv_count",        pv_count);
	output << dump_row ("lv_count",        lv_count);
	output << dump_row ("vg_attr",         vg_attr);
	output << dump_row ("vg_extent_count", vg_extent_count);
	output << dump_row ("vg_free_count",   vg_free_count);
	output << dump_row ("vg_seqno",        vg_seqno);

	return output.str();
}


