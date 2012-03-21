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


#include <list>
#include <string>
#include <sstream>

#include "linear.h"
#include "main.h"
#include "mirror.h"
#include "segment.h"
#include "stripe.h"
#include "volumegroup.h"
#include "volume.h"

#include "utils.h"
#include "log.h"

//RAR lazy
std::map<std::string, VolumeGroup*> vg_lookup;
std::map<std::string, Volume*> vol_lookup;
std::map<std::string, Segment*> vg_seg_lookup;
std::map<std::string, Segment*> vol_seg_lookup;

/**
 * VolumeGroup
 */
VolumeGroup::VolumeGroup (void) :
	pv_count (0),
	lv_count (0),
	vg_extent_count (0),
	vg_free_count (0),
	vg_seqno (0)
{
	type.push_back ("volumegroup");
}

/**
 * ~VolumeGroup
 */
VolumeGroup::~VolumeGroup()
{
}


/**
 * dump_vol
 */
void dump_vol (void)
{
	std::map<std::string,Volume*>::iterator i;
	log_debug ("Volumes map (vol_lookup):\n");
	for (i = vol_lookup.begin(); i != vol_lookup.end(); i++) {
		Volume *v = (*i).second;
		log_debug ("\t%s => %s\n", (*i).first.c_str(), v->name.c_str());
	}
	log_debug ("\n");
}

/**
 * dump_vol_seg
 */
void dump_vol_seg (void)
{
	std::map<std::string,Segment*>::iterator i;
	log_debug ("Volume Segments map (vol_seg_lookup):\n");
	for (i = vol_seg_lookup.begin(); i != vol_seg_lookup.end(); i++) {
		Segment *s = (*i).second;
		log_debug ("\t%s => %s\n", (*i).first.c_str(), s->name.c_str());
	}
	log_debug ("\n");
}

/**
 * dump_vol_group
 */
void dump_vol_group (void)
{
	std::map<std::string,VolumeGroup*>::iterator i;

	log_debug ("Volume Group map (vg_lookup):\n");
	for (i = vg_lookup.begin(); i != vg_lookup.end(); i++) {
		VolumeGroup *vg = (*i).second;
		log_debug ("\t%s => %s\n", (*i).first.c_str(), vg->name.c_str());
	}
	log_debug ("\n");
}

/**
 * dump_vol_group_seg
 */
void dump_vol_group_seg (void)
{
	std::map<std::string,Segment*>::iterator i;

	log_debug ("Volume Group Segments map (vg_seg_lookup):\n");
	for (i = vg_seg_lookup.begin(); i != vg_seg_lookup.end(); i++) {
		Segment *s = (*i).second;
		log_debug ("\t%s => %s\n", (*i).first.c_str(), s->name.c_str());
	}
	log_debug ("\n");
}


/**
 * fd_probe_children
 */
void fd_probe_children (DPContainer *item)
{
	// recurse through children and add_probe()
	if (item->children.size() == 0) {
		//log_info ("PROBE: %s\n", item->name.c_str());
		queue_add_probe (item);
	}

	for (unsigned int i = 0; i < item->children.size(); i++) {
		fd_probe_children (item->children[i]);
	}
}

/**
 * fd_vgs - Create the VolumeGroup objects
 */
void fd_vgs (DPContainer &disks)
{
	std::string command;
	std::vector<std::string> output;
	std::string error;
	std::map<std::string,StringNum> tags;
	VolumeGroup *vg = NULL;
	unsigned int i;

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

	command = "vgs --unquoted --separator='\t' --units=b --nosuffix --nameprefixes --noheadings --options vg_name,pv_count,lv_count,vg_attr,vg_size,vg_free,vg_uuid,vg_extent_size,vg_extent_count,vg_free_count,vg_seqno,pv_name";
	execute_command (command, output);

	for (i = 0; i < output.size(); i++) {
		parse_tagged_line ((output[i]), "\t", tags);

		std::string vg_uuid = tags["LVM2_VG_UUID"];
		std::string pv_name = tags["LVM2_PV_NAME"];

		vg = vg_lookup[vg_uuid];
		if (vg == NULL) {
			vg = new VolumeGroup;
			vg->parent = &disks;
			//vg->device = "/dev/dm-0"; //RAR what?
			// VolumeGroup doesn't have a dedicated device, so it would have to delegate to the vg segment

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
		}

		//std::string seg_id = vg->name + ":" + pv_name;
		//log_debug ("\t%s\n", seg_id.c_str());

		DPContainer *cont = disks.find_device (pv_name);
		//log_debug ("cont for %s = %p\n", pv_name.c_str(), cont);

		Segment *vg_seg = new Segment;
		vg_seg->bytes_size = cont->bytes_size;
		//RAR vg_seg->type.push_back ("vg segment");
		vg_seg->block_size = vg->block_size;
		vg_seg->uuid = vg_uuid;
		vg_seg->name = vg->name;

		//log_debug ("vg block size = %ld\n", vg->block_size);

		cont->add_child (vg_seg);

		//log_debug ("lookup uuid %s\n", vg_uuid.c_str());
		vg->add_segment (vg_seg);

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

	//dump_vol_group();
	//dump_vol_group_seg();
}

/**
 * fd_pvs - Attach all the Segments (VolumeGroup and Volumes)
 */
void fd_pvs (DPContainer &disks)
{
	std::string command;
	std::vector<std::string> output;
	std::string error;
	unsigned int i;
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

	command = "pvs --unquoted --separator='\t' --units=b --nosuffix --nameprefixes --noheadings --options pv_name,lv_name,lv_uuid,lv_attr,vg_uuid,vg_name,segtype,pvseg_start,pvseg_size";
	execute_command (command, output);

	for (i = 0; i < output.size(); i++) {
		parse_tagged_line ((output[i]), "\t", tags);

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
		std::string start   = tags["LVM2_PVSEG_START"];	// physical volume offset, i.e. how far into /dev/loop0?

		if ((lv_attr[0] == 'i') || (lv_attr[0] == 'l')) {		// mirror image/log
			lv_name = lv_name.substr (1, lv_name.length() - 2);	// Strip the []'s
		}

		//std::string seg_id = vg_name + ":" + lv_name + ":" + pv_name + "(" + start + ")";
		std::string seg_id = pv_name + "(" + start + ")";
		//log_debug ("\tseg_id = %s\n", seg_id.c_str());

		//log_debug ("lookup uuid %s\n", vg_uuid.c_str());
		VolumeGroup *vg = vg_lookup[vg_uuid];
		//log_debug ("vg extent size = %ld\n", vg->block_size);

		Segment *vg_seg = vg_seg_lookup[dev];	//XXX this should exist
		if (!vg_seg)
			continue;			//XXX error?

		Segment *vol_seg = new Segment;
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
		vol_seg_lookup[seg_id] = vol_seg;

		std::string vol_id = pv_name + "(" + start + ")";	//XXX dupe
	}
	//log_debug ("\n");

	//dump_vol_seg();
}

/**
 * fd_lvs - Build the Volumes from the Segments
 */
void fd_lvs (DPContainer &disks)
{
	std::string command;
	std::vector<std::string> output;
	std::string error;
	unsigned int i;
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

	command = "lvs --all --unquoted --separator='\t' --units=b --nosuffix --noheadings --nameprefixes --sort lv_kernel_minor --options vg_uuid,vg_name,lv_name,lv_attr,mirror_log,lv_uuid,lv_size,lv_path,lv_kernel_major,lv_kernel_minor,seg_count,segtype,stripes,stripe_size,seg_start_pe,seg_pe_ranges,devices";
	execute_command (command, output);

	//log_debug ("Volumes:\n");
	for (i = 0; i < output.size(); i++) {
		parse_tagged_line ((output[i]), "\t", tags);

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

		Volume *v = vol_lookup[vol_id];
		if (v == NULL) {
			if (lv_type == "linear") {
				v = new Linear;
			} else if (lv_type == "striped") {
				v = new Stripe;
			} else if (lv_type == "mirror") {
				v = new Mirror;
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

		std::vector<std::string>::iterator it_dev;
		for (it_dev = device_list.begin(); it_dev != device_list.end(); it_dev++) {
			std::string dep = (*it_dev);
			//log_debug ("\t\t%s\n", dep.c_str());

			std::map<std::string, Segment*>::iterator it_vol_seg;
			it_vol_seg = vol_seg_lookup.find (dep);
			if (it_vol_seg != vol_seg_lookup.end()) {
				//RAR connect up stuff
				Segment *s_dep = (*it_vol_seg).second;
				v->add_segment (s_dep);
				vol_seg_lookup.erase (it_vol_seg);
				continue;
			}

			dep = vg_name + ":" + (*it_dev);
			size_t pos = dep.find ('(');
			if (pos != std::string::npos) {
				dep = dep.substr (0, pos);
			}
			std::map<std::string, Volume*>::iterator it_vol;
			it_vol = vol_lookup.find (dep);
			if (it_vol != vol_lookup.end()) {
				Volume *v_dep = (*it_vol).second;
				//RAR connect up stuff
				v->add_child (v_dep);
				vol_lookup.erase (it_vol);
				continue;
			}

			log_error ("DEP: couldn't find %s\n", dep.c_str());
		}
	}

	//dump_vol_seg();
	//dump_vol();

	//transfer volumes to disks
	std::map<std::string, Volume*>::iterator it_vol;
	for (it_vol = vol_lookup.begin(); it_vol != vol_lookup.end(); it_vol++) {
		DPContainer *c = (*it_vol).second;
		//log_debug ("volume %s (%s)\n", c->name.c_str(), c->parent->name.c_str());
		c->parent->add_child (c);
		fd_probe_children (c);
	}
}

/**
 * find_devices
 */
void VolumeGroup::find_devices (DPContainer &disks)
{
	fd_vgs (disks);
	fd_pvs (disks);
	fd_lvs (disks);
}


/**
 * dump_dot
 */
std::string VolumeGroup::dump_dot (void)
{
	std::ostringstream output;

	output << dump_table_header ("VolumeGroup", "#008080");

	output << Whole::dump_dot();

	output << dump_row ("pv_count",        pv_count);
	output << dump_row ("lv_count",        lv_count);
	output << dump_row ("vg_attr",         vg_attr);
	output << dump_row ("vg_extent_count", vg_extent_count);
	output << dump_row ("vg_free_count",   vg_free_count);
	output << dump_row ("vg_seqno",        vg_seqno);

	output << dump_table_footer();
	output << dump_dot_children();

	return output.str();
}

