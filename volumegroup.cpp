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


#include <stdio.h>

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

struct queue_item {
	Container *item;
	std::list<std::string> requires;
};

//RAR lazy
std::map<std::string, VolumeGroup*> vg_lookup;
std::map<std::string, Volume*> vol_lookup;

std::map<std::string, Segment*> vg_seg_lookup;
std::map<std::string, Segment*> vol_seg_lookup;

std::map<std::string,Container*> q_easy;
std::map<std::string, struct queue_item> q_hard;

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
	type = "volumegroup";
}

/**
 * ~VolumeGroup
 */
VolumeGroup::~VolumeGroup()
{
}


/**
 * dump_easy
 */
void dump_easy (void)
{
	std::map<std::string,Container*>::iterator i;

	log_debug ("Easy Queue map (q_easy):\n");
	for (i = q_easy.begin(); i != q_easy.end(); i++) {
		Container *c = (*i).second;
		log_debug ("\t%s => %s (%s)\n", (*i).first.c_str(), c->name.c_str(), c->type.c_str());
		for (unsigned int n = 0; n < c->children.size(); n++) {
			log_debug ("\t\t%s\n", c->children[n]->name.c_str());
		}
	}
	log_debug ("\n");
}

/**
 * dump_hard
 */
void dump_hard (void)
{
	std::map<std::string, struct queue_item>::iterator i;

	log_debug ("Hard Queue map (q_hard):\n");
	for (i = q_hard.begin(); i != q_hard.end(); i++) {
		std::string name = (*i).first;
		Whole *w = static_cast <Whole*> ((*i).second.item);
		log_debug ("%s (%s) - (%lu,%lu)\n", name.c_str(), (*i).second.item->type.c_str(), w->segments.size(), w->children.size());
		std::list<std::string>::iterator j;
		for (j = (*i).second.requires.begin(); j != (*i).second.requires.end(); j++) {
			log_debug ("\t%s\n", (*j).c_str());
		}
	}
	log_debug ("\n");
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
 * fd_vgs - Create the VolumeGroup objects
 */
void fd_vgs (Container &disks)
{
	std::string command;
	std::string output;
	std::string error;
	std::vector<std::string> lines;
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
	execute_command (command, output, error);

	lines.clear();
	explode ("\n", output, lines);

	for (i = 0; i < lines.size(); i++) {
		parse_tagged_line ((lines[i]), "\t", tags);

		std::string vg_uuid = tags["LVM2_VG_UUID"];
		std::string pv_name = tags["LVM2_PV_NAME"];

		vg = vg_lookup[vg_uuid];
		if (vg == NULL) {
			vg = new VolumeGroup;
			vg->parent = &disks;
			vg->device = "/dev/dm-0"; //RAR what?
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

		Container *cont = disks.find_device (pv_name);
		//log_debug ("cont for %s = %p\n", pv_name.c_str(), cont);

		Segment *vg_seg = new Segment;
		vg_seg->bytes_size = cont->bytes_size;
		vg_seg->type = "vg segment";
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
void fd_pvs (Container &disks)
{
	std::string command;
	std::string output;
	std::string error;
	unsigned int i;
	std::vector<std::string> lines;
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
	execute_command (command, output, error);

	lines.clear();
	explode ("\n", output, lines);

	for (i = 0; i < lines.size(); i++) {
		parse_tagged_line ((lines[i]), "\t", tags);

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
void fd_lvs (Container &disks)
{
	std::string command;
	std::string output;
	std::string error;
	unsigned int i;
	std::vector<std::string> lines;
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
	execute_command (command, output, error);

	lines.clear();
	explode ("\n", output, lines);

	//log_debug ("Volumes:\n");
	for (i = 0; i < lines.size(); i++) {
		parse_tagged_line ((lines[i]), "\t", tags);

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
			//v->parent = vg_lookup[vg_uuid];	//XXX until we know better

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
		disks.add_child ((*it_vol).second);
	}
}

/**
 * fd_stitch - Assemble all the pieces
 */
void fd_stitch (Container &disks)
{
	std::map<std::string, struct queue_item>::iterator it1;
	std::list<std::string>::iterator it2;

	return;
	dump_easy();
	dump_hard();

	for (it1 = q_hard.begin(); it1 != q_hard.end(); ) {
		Whole *w = static_cast<Whole*> ((*it1).second.item);
		log_debug ("%s (%s)\n", w->name.c_str(), w->type.c_str());

#if 0 // dump object
		std::string parent;
		if (w->parent)
			parent = "(" + w->parent->name + ")";

		log_debug ("\tname          = %s\n",   l->name.c_str());
		log_debug ("\tuuid          = %s\n",   l->uuid.c_str());
		log_debug ("\tdevice        = %s\n",   l->device.c_str());
		log_debug ("\tparent offset = %lld\n", l->parent_offset);
		log_debug ("\tblock size    = %ld\n",  l->block_size);
		log_debug ("\tbytes size    = %lld\n", l->bytes_size);
		log_debug ("\tbytes used    = %lld\n", l->bytes_used);
		log_debug ("\tparent        = %p%s\n", l->parent, parent.c_str());
		log_debug ("\tchildren      = %lu\n",  l->children.size());
		// list children
		log_debug ("\tlv_attr       = %s\n",   l->lv_attr.c_str());
		log_debug ("\tkernel major  = %ld\n",  l->kernel_major);
		log_debug ("\tkernel minor  = %ld\n",  l->kernel_minor);
		log_debug ("\tsegments      = %lu\n",  l->segments.size());
		// list segments
#endif

		log_debug ("requires:\n");
		// for each requirement
		for (it2 = (*it1).second.requires.begin(); it2 != (*it1).second.requires.end(); ) {
			std::string req = (*it2);
			log_debug ("\t%s\n", req.c_str());

			std::map<std::string,Container*>::iterator itq;
			itq = q_easy.find (req);

			// does it exist in q_easy?
			if (itq == q_easy.end()) {
				log_error ("missing dep %s\n", req.c_str());
				it2++;
				break;
			}

			Container *c = (*itq).second;

			std::string type = c->type;
			if (type == "segment") {
				//XXX all very risky
				// add_segment
				Segment *seg = static_cast<Segment*> ((*itq).second);
				w->add_segment (seg);
			} else {
				// or add_child
				w->add_child (c);
			}

			// delete requirement from q_easy
			q_easy.erase (itq);

			// delete requirement for our list
			(*it1).second.requires.erase (it2++);
		}

		//q_hard.erase (it1++);

		// is requirement list empty?
		// no - continue
		// yes
		// copy item into q_easy
		// delete item from q_hard

		// what about parents? volumes? volume groups?
	}

	log_error ("----------------------------------\n");
	dump_easy();
	dump_hard();

	return;
#if 0
	for (unsigned int p = 0; p < 1; p++) {		// XXX 1 is the complexity of the arrangement of the segments
		if (q_hard.size() == 0)
			break;
#endif
#if 0
	map<string, SerialdMsg::SerialFunction_t>::iterator pm_it = port_map.begin();
	while(pm_it != port_map.end())
	{
		if (pm_it->second == delete_this_id) {
			port_map.erase(pm_it++);	// Use iterator.
							// Note the post increment.
							// Increments the iterator but returns the
							// original value for use by erase
		} else {
			++pm_it;			// Can use pre-increment in this case
							// To make sure you have the efficient version
		}
	}
#endif


#if 0
		//XXX iterate backwards so we can erase things?
		std::vector<struct queue_item>::iterator it6;
		for (it6 = q_hard.begin(); it6 != q_hard.end(); it6++) {
			bool found_all_hard_stuff = true;
			for (unsigned int q = 0; q < (*it6).requires.size(); q++) {
				std::string req_name = (*it6).requires[q];
				Container *req = q_easy[req_name];
				if (req != NULL) {
					log_debug ("found req\n");
					(*it6).item->add_child (req);
					q_easy.erase (q_easy.find(req_name));
				} else {
					log_debug ("DIDN'T found req: %s\n", req_name.c_str());
					found_all_hard_stuff = false;
				}
			}
			if (found_all_hard_stuff) {
				q_easy["done"] = (*it6).item;
				//q_hard.erase (it6);	//RAR can't do erase in for loop!
			}
		}
#endif

#ifdef JUNK
		bool found_everything = true;
#if 0
		for (unsigned int k = 0; k < device_list.size(); k++) {
			std::string seg_id = vg_name + ":" + lv_name + ":" + device_list[k];
			Segment *vol_seg = vol_seg_lookup[seg_id];
			vol_seg_lookup.erase (vol_seg_lookup.find(seg_id));
			//log_debug ("\t%s ", lv_attr.c_str());
			if (vol_seg) {
				//log_debug ("\t%s => \e[32m%s\e[0m\n", seg_id.c_str(), vol_seg->name.c_str());
				if (lv_type == "striped") {
					Linear *lin = new Linear;
					lin->name = vol_seg->name; // XXX copy other details
					item->add_child (lin);
					// need to link to segments
				}
			} else {
				found_everything = false;
				//log_debug ("\t%s => \e[31m%s\e[0m\n", seg_id.c_str(), "MISSING");
			}
		}
#endif
		// search for all segments
		// all found?
		//	yes -> easy
		//	no  -> hard

		std::string sstart = tags["LVM2_SEG_START_PE"];
		//log_debug ("SEG_START = %s\n", sstart.c_str());
		std::string seg_id = vg_name + ":" + lv_name + "(" + sstart + ")"; //RAR uniq?
		if (found_everything) {
			q_easy[seg_id] = item;
		} else {
			struct queue_item i = { seg_id, item, device_list };
			for (unsigned int m = 0; m < i.requires.size(); m++) {
				// prefix with volume name
				i.requires[m] = vg_name + ":" + i.requires[m];
			}
			q_hard.push_back (i);
		}

		continue;
#if 0
		if ((lv_attr[0] == 'i') || (lv_attr[0] == 'l')) { // mirror (i)mage or (l)og
			// De-mangle the lv_name
			size_t ending = 0;
			if (lv_attr[0] == 'i') {
				ending = lv_name.rfind ("_mimage_");
			} else {
				ending = lv_name.rfind ("_mlog");
			}

			lv_name = lv_name.substr (1, ending - 1);
			//log_debug ("lv_name = %s\n", lv_name.c_str());
		}
#endif
		vg = vg_lookup[vg_uuid];			//log_debug ("\tlookup %s  vg = %s\n", vg_uuid.c_str(), vg->name.c_str());

		vol = static_cast<Volume*>(vg->find_name (lv_name));
		if (!vol) {
			//log_debug ("new volume\n");
			vol = new Volume;
			vol->name    = lv_name;
			vol->uuid    = lv_uuid;	//RAR except for mirrors
			vol->lv_attr = lv_attr;
			vg->add_child (vol);

			vol->device       = tags["LVM2_LV_PATH"];
			vol->bytes_size   = tags["LVM2_LV_SIZE"];
			vol->kernel_major = tags["LVM2_LV_KERNEL_MAJOR"];
			vol->kernel_minor = tags["LVM2_LV_KERNEL_MINOR"];
		}

		//log_debug ("vol = %p\n", vol);

		std::string mlog = tags["LVM2_MIRROR_LOG"];

		std::string seg_count     = tags["LVM2_SEG_COUNT"];	//log_debug ("\tseg count = %s\n", seg_count.c_str());
		std::string segtype       = tags["LVM2_SEGTYPE"];	//log_debug ("\tseg type = %s\n", segtype.c_str());
		long        stripes       = tags["LVM2_STRIPES"];	//log_debug ("\tstripes = %ld\n", stripes);
		std::string stripe_size   = tags["LVM2_STRIPE_SIZE"];	//log_debug ("\tstripe size = %s\n", stripe_size.c_str());
		std::string seg_pe_ranges = tags["LVM2_SEG_PE_RANGES"];

		//long long seg_start = tags ["LVM2_SEG_START"]
		//log_debug ("\tseg start = %s\n", seg_start.c_str());

		//long long seg_size = tags["LVM2_SEG_SIZE"];
		//log_debug ("\tseg size = %s\n", seg_size.c_str());

		std::string pe_device;
		int pe_start  = -1;
		int pe_finish = -1;

		if (lv_attr[0] == 'm') {		// (m)irror
			stripes = 0;			//XXX don't create another segment
		}

		for (int s = 0; s < stripes; s++) {
			extract_dev_range (seg_pe_ranges, pe_device, pe_start, pe_finish, s);
			//log_debug ("seg pe ranges = %s, %d, %d\n", pe_device.c_str(), pe_start, pe_finish);

			Segment *vg_seg = vg_seg_lookup[pe_device];
			//log_debug ("vg_seg = %p\n", vg_seg);

			if ((lv_attr[0] == 'i') || (lv_attr[0] == 'l')) { // mirror (i)mage or (l)og
				// Store a reference for later
				std::string mirr_name = tags["LVM2_LV_NAME"];
				mirr_name = mirr_name.substr (1, mirr_name.size() - 2);
				//log_debug ("storing mirror name: %s\n", mirr_name.c_str());
				vg_seg_lookup[mirr_name] = vg_seg;
			}

			Segment *vol_seg = new Segment;
			vol_seg->type = "volume";

			vol_seg->block_size = vg->block_size;
			vol_seg->bytes_size = vol_seg->block_size * (pe_finish - pe_start);
			vol_seg->bytes_used = vol_seg->bytes_size;

			//vol_seg->volume_offset = seg_start;
			vol_seg->device        = pe_device;
			//vol_seg->segment_size  = seg_size;
			vol_seg->parent_offset = pe_start * vg->block_size;
			//log_debug ("volume offset = %lld, device = %s, seg size = %lld, device offset = %lld\n", vol_seg.volume_offset, vol_seg.device.c_str(), vol_seg.segment_size, vol_seg.parent_offset);

			vol_seg->name = lv_name;
			vol_seg->uuid = vol->uuid;
			vol_seg->whole = vol;

			vg_seg->add_child (vol_seg);
			vol->add_segment (vol_seg);
		}

		//log_debug ("\n");
		//log_debug ("vg = %s   lv = %s\n", tmp1.c_str(), vol->name.c_str());
	}
#endif // JUNK

#if 0
	std::map<std::string,Container*>::iterator it9;
	for (it9 = q_easy.begin(); it9 != q_easy.end(); it9++) {
		Container *c1 = (*it9).second;
		c1->parent->add_child (c1);
		//vg->add_child (c1);
	}
#endif
}

/**
 * find_devices
 */
void VolumeGroup::find_devices (Container &disks)
{
	fd_vgs (disks);
	fd_pvs (disks);
	fd_lvs (disks);
#if 0
	fd_stitch (disks);
#endif
}


/**
 * dump
 */
void VolumeGroup::dump (int indent /* = 0 */)
{
	std::string size = get_size (bytes_size);
	std::string free = get_size (bytes_size - bytes_used);

	iprintf (indent, "%s (%s)\n", name.c_str(), size.c_str());

	iprintf (indent+8, "pv_count:        %ld\n", pv_count);
	iprintf (indent+8, "vg_free:         %s\n", free.c_str());
	iprintf (indent+8, "vg_uuid:         %s\n", uuid.c_str());

	//iprintf (indent+8, "vg_attr:         %s\n",   vg_attr.c_str());
	//iprintf (indent+8, "vg_name:         %s\n",   vg_name.c_str());
	//iprintf (indent+8, "lv_count:        %d\n",   lv_count);
	//iprintf (indent+8, "vg_size:         %lld\n", vg_size);
	//iprintf (indent+8, "vg_extent_count: %lld\n", vg_extent_count);
	//iprintf (indent+8, "vg_free_count:   %lld\n", vg_free_count);
	//iprintf (indent+8, "vg_seqno:        %ld\n",  vg_seqno);

	Whole::dump (indent);
}

/**
 * dump_csv
 */
void VolumeGroup::dump_csv (void)
{
	Whole::dump_csv();
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

